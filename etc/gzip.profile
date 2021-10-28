# Firejail profile for gzip
# This file is overwritten after every install/update
quiet
# Persistent local customizations
include /etc/firejail/gzip.local
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

private-dev

include /etc/firejail/default.profile
