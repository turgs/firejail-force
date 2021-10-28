# Firejail profile for wine
# This file is overwritten after every install/update
# Persistent local customizations
include /etc/firejail/wine.local
# Persistent global definitions
include /etc/firejail/globals.local

noblacklist ${HOME}/.Steam
noblacklist ${HOME}/.local/share/Steam
noblacklist ${HOME}/.local/share/steam
noblacklist ${HOME}/.steam
noblacklist ${HOME}/.wine

include /etc/firejail/disable-common.inc
include /etc/firejail/disable-devel.inc
include /etc/firejail/disable-programs.inc

caps.drop all
netfilter
nodvd
nogroups
nonewprivs
noroot
notv
seccomp
