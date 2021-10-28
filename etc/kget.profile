# Firejail profile for kget
# This file is overwritten after every install/update
# Persistent local customizations
include /etc/firejail/kget.local
# Persistent global definitions
include /etc/firejail/globals.local

noblacklist ${HOME}/.kde/share/apps/kget
noblacklist ${HOME}/.kde/share/config/kgetrc
noblacklist ${HOME}/.kde4/share/apps/kget
noblacklist ${HOME}/.kde4/share/config/kgetrc

include /etc/firejail/disable-common.inc
include /etc/firejail/disable-devel.inc
include /etc/firejail/disable-passwdmgr.inc
include /etc/firejail/disable-programs.inc

include /etc/firejail/whitelist-var-common.inc

caps.drop all
netfilter
nodvd
nogroups
nonewprivs
noroot
nosound
notv
novideo
protocol unix,inet,inet6
seccomp

private-dev
private-tmp

# memory-deny-write-execute
noexec ${HOME}
noexec /tmp
