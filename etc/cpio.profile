# Firejail profile for cpio
# This file is overwritten after every install/update
quiet
# Persistent local customizations
include /etc/firejail/cpio.local
# Persistent global definitions
include /etc/firejail/globals.local

blacklist /run/user/*/bus
blacklist /tmp/.X11-unix

noblacklist /sbin
noblacklist /usr/sbin

include /etc/firejail/disable-common.inc
include /etc/firejail/disable-passwdmgr.inc
include /etc/firejail/disable-programs.inc

caps.drop all
net none
no3d
nodvd
nonewprivs
nosound
notv
novideo
seccomp
shell none
tracelog

private-dev
