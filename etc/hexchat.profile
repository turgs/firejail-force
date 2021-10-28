# Firejail profile for hexchat
# This file is overwritten after every install/update
# Persistent local customizations
include /etc/firejail/hexchat.local
# Persistent global definitions
include /etc/firejail/globals.local

noblacklist ${HOME}/.config/hexchat
# noblacklist /usr/lib/python2*
# noblacklist /usr/lib/python3*

include /etc/firejail/disable-common.inc
include /etc/firejail/disable-devel.inc
include /etc/firejail/disable-programs.inc

mkdir ${HOME}/.config/hexchat
whitelist ${HOME}/.config/hexchat
include /etc/firejail/whitelist-common.inc
include /etc/firejail/whitelist-var-common.inc

caps.drop all
machine-id
netfilter
no3d
nodvd
nogroups
nonewprivs
noroot
nosound
notv
novideo
protocol unix,inet,inet6
seccomp
shell none
tracelog

disable-mnt
# debug note: private-bin requires perl, python, etc on some systems
private-bin hexchat,python*
private-dev
#private-lib - python problems
private-tmp

# memory-deny-write-execute - breaks python
noexec ${HOME}
noexec /tmp
