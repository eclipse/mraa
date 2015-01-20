Debugging libmraa                         {#debugging}
=================

This page contains a few debugging tip, when filing an issue please go through
this list as if you don't it's the first thing we'll have to ask you to do.

### Version

Getting the exact version of libmraa you're running is really important to us.
The best way to get this is to call mraa_get_version() or mraa.getVersion(). If
mraa returns x.x.x-dirty then your version was not built from a git tree or you
built out of tree (see our building doc) - or you don't have git installed.

### Syslog

mraa logs pretty much everything that goes wrong to syslog, these days it'll go
to the systemd journal usually so check with `sudo journalctl -f` whilst
running your app or check all libmraa messages with `journalctl -t libmraa`.
Grab all the messages from the initialisation message right up to your last
one, using a pastebin is always a good idea, I like dpaste.com. If your system
does not have systemd likely your log is in /var/log/messages or similar.

### Common Errors

* Not running as root
* Incorrect IO numbers, mraa uses physical connector pin numbering see your
  platform doc for details
* Unsuported platform
* Using the wrong pin, check pin capabilities

