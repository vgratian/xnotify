
## Xnotify

Xnotify is a notification utility for Xorg, fine-tuned for tiling window managers, such as [dwm](https://dwm.suckless.org/). It is similar to tools like [xmessage](https://www.x.org/releases/X11R7.6/doc/man/man1/xmessage.1.xhtml) and [libnotify](https://gitlab.gnome.org/GNOME/libnotify). However, I tried to make it as simple and minimal possible - with no daemons, no sockets, no dbus.

Here are two examples, in English and Armenian:

![example](xnotify.gif "example")

## Features

Xnotify is inspired by the suckless philosophy and it tries to do only one thing: show a notification message. At the same time, I tried to make it fully customizeable. Here are the main highlights:

- Size of the pop-up window, its position, colors and fonts are customizeable in `config.h`
- Any Xft-compatible font is OK, you can run `fc-list` to see what's available on your system
- Optionally, Xnotify can log all notifications to a file, see details below

## Requrements

Building xnotify requires:
- `pkg-config` (`pkgconf` on Manjaro)
- `libX11` (`libx11`)
- `libXft` (`libxft`)
- `freetype2` (`freetype-2`)

Additionally, `fontconfig`, allows you to navigate and select Xft-compatible fonts (e.g. `fc-list`).

## Build

- Run `make config.h` to create a copy of the default config file
- Edit `config.h` and customize if you wish
- Run `make` to build the program

(I hate `sudo make install`, therefore I did not add that rule.)

## Usage

Each notification consists of `subject` and `message`, the former is optional. An example invocation with subject:

```sh
$ xnotify alert "battery level low"
```

Example with no subject:
```sh
$ xnotify "battery level low"
```

Xnotify will fork and close stdout and stderr, if you wan't to prevent that, use the `-d` flag. Example:

```sh
$ xnotify -d alert "battery level low"
```

## Logging

Xnotify will log all notifications to a log file, if `LOGFN` and `LOGMAXS` are defined in `config.h`. This is **disabled** by default. Each line is one notification in the following format:

* `DATE` `TIME` [`PPID`:`PNAME`] (`SUBJECT`) `MESSAGE`

where `PPID` and `PNAME` are PID and name of the process that invoked Xnotify. If Xnotify can't figure out `PNAME`, its value will be `-`. Similarly, subject will be `-` if notification had no subject.

## Muting and unmuting

Xnotify can be muted and unmuted with the flags `-m` and `-u` respectively. When muted, xnotify will not display notification, but will log them if enabled.
You can query if it's muted with `xnotify -s`.

## Bugs & To-dos

* Xft will not complain, if an invalid font name is used, instead it will load something else that's unavailable on your system. If the text is not properly drawn or you see squares instead of letters, make sure font names in `config.h` are correct or try some other font.

* If message size exceeds window size, it is trimmed. Same needs to be done with subject.

* With multiple notifications, sometimes they overlap (know why this happens, just need to fix it).


## Contributing

If you used this utiltiy, I would appreciate any feedback, and you are more than welcome to open an issue or merge request.
