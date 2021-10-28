# Firejail profile for strings
# This file is overwritten after every install/update
quiet
# Persistent local customizations
include /etc/firejail/strings.local
# Persistent global definitions
include /etc/firejail/globals.local

blacklist /run/user/*/bus
blacklist /tmp/.X11-unix

ignore noroot
net none
no3d
nodvd
nosound
notv
novideo
shell none
tracelog

private-bin strings
private-dev
private-lib

memory-deny-write-execute

include /etc/firejail/default.profile
