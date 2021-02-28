package main

import (
	"flag"
	"log"
	"os"
	"os/exec"
	"syscall"

	"github.com/pkg/errors"
	"github.com/pkg/term/termios"
	"golang.org/x/sync/errgroup"
	"golang.org/x/sys/unix"
)

func setupDriver(ptym *os.File, driver string) error {
	cmd := exec.Command(driver)

	cmd.Stdin = ptym
	cmd.Stdout = ptym

	if err := cmd.Start(); err != nil {
		return errors.Wrap(err, "driver start failed")
	}

	if err := cmd.Wait(); err != nil {
		return errors.Wrap(err, "driver wait failed")
	}

	return nil
}

func setupProg(pts *os.File, prog string) error {
	cmd := exec.Command(prog)

	cmd.Stdin = pts
	cmd.Stdout = pts

	var attr unix.Termios
	if err := termios.Tcgetattr(pts.Fd(), &attr); err != nil {
		return errors.Wrap(err, "prog tcgetattr failed")
	}
	attr.Lflag &^= (syscall.ECHO)
	attr.Oflag &^= (syscall.ONLCR)

	if err := termios.Tcsetattr(pts.Fd(), termios.TCSANOW, &attr); err != nil {
		return errors.Wrap(err, "prog tcsetattr failed")
	}

	if err := cmd.Start(); err != nil {
		return errors.Wrap(err, "prog start failed")
	}

	if err := cmd.Wait(); err != nil {
		return errors.Wrap(err, "prog wait failed")
	}

	return nil
}

func main() {
	driver := flag.String("d", "", "driver program")
	prog := flag.String("p", "", "answer program")
	flag.Parse()

	if *driver == "" || *prog == "" {
		log.Fatal("specify driver and program")
	}

	ptym, pts, err := termios.Pty()
	if err != nil {
		log.Fatal(err)
	}

	eg := errgroup.Group{}
	eg.Go(func() error {
		return setupProg(pts, *prog)
	})
	eg.Go(func() error {
		return setupDriver(ptym, *driver)
	})

	if err := eg.Wait(); err != nil {
		log.Fatal(err)
	}
}
