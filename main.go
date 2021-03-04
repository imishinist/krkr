package main

import (
	"context"
	"flag"
	"log"
	"os"
	"os/exec"
	"syscall"
	"time"

	"github.com/pkg/errors"
	"github.com/pkg/term/termios"
	"golang.org/x/sync/errgroup"
	"golang.org/x/sys/unix"
)

func setNoEcho(tty *os.File) error {
	var attr unix.Termios
	if err := termios.Tcgetattr(tty.Fd(), &attr); err != nil {
		return errors.Wrap(err, "prog tcgetattr failed")
	}
	attr.Lflag &^= (syscall.ECHO)
	attr.Oflag &^= (syscall.ONLCR)

	if err := termios.Tcsetattr(tty.Fd(), termios.TCSANOW, &attr); err != nil {
		return errors.Wrap(err, "prog tcsetattr failed")
	}
	return nil
}

func run(ctx context.Context, tty *os.File, prog string, asSlave bool) error {
	cmd := exec.CommandContext(ctx, prog)

	cmd.Stdin = tty
	cmd.Stdout = tty

	if asSlave {
		if err := setNoEcho(tty); err != nil {
			return errors.Wrap(err, "set noecho error")
		}
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
	timeout := flag.Duration("dur", time.Second*5, "")
	flag.Parse()

	if *driver == "" || *prog == "" {
		log.Fatal("specify driver and program")
	}

	ptym, pts, err := termios.Pty()
	if err != nil {
		log.Fatal(err)
	}

	ctx, cancel := context.WithTimeout(context.Background(), *timeout)
	defer cancel()

	eg, ctx := errgroup.WithContext(ctx)
	eg.Go(func() error {
		return run(ctx, pts, *prog, false)
	})
	eg.Go(func() error {
		return run(ctx, ptym, *driver, true)
	})

	if err := eg.Wait(); err != nil {
		log.Fatal(err)
	}
}
