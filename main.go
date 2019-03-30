package main

import (
	"flag"
	"log"
	"os"
	"os/exec"
	"syscall"

	"github.com/pkg/errors"
	"github.com/pkg/term/termios"
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

	var attr syscall.Termios
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

	errCh := make(chan error)

	go func() {
		errCh <- setupProg(pts, *prog)
	}()
	go func() {
		errCh <- setupDriver(ptym, *driver)
	}()

	for i := 0; i < 2; i++ {
		select {
		case err := <-errCh:
			if err != nil {
				log.Fatal(err)
			}
		}
	}

}
