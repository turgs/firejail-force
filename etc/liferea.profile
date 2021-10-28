# Firejail profile for liferea
# This file is overwritten after every install/update
# Persistent local customizations
include /etc/firejail/liferea.local
# Persistent global definitions
include /etc/firejail/globals.local

noblacklist ${HOME}/.cache/liferea
noblacklist ${HOME}/.config/liferea
noblacklist ${HOME}/.local/share/liferea

include /etc/firejail/disable-common.inc
include /etc/firejail/disable-devel.inc
include /etc/firejail/disable-passwdmgr.inc
include /etc/firejail/disable-programs.inc

mkdir ${HOME}/.cache/liferea
mkdir ${HOME}/.config/liferea
mkdir ${HOME}/.local/share/liferea
whitelist ${HOME}/.cache/liferea
whitelist ${HOME}/.config/liferea
whitelist ${HOME}/.local/share/liferea
include /etc/firejail/whitelist-common.inc

caps.drop all
netfilter
# no3d
nodvd
nogroups
nonewprivs
noroot
# nosound
notv
novideo
protocol unix,inet,inet6
seccomp
shell none

disable-mnt
private-dev
private-tmp

noexec ${HOME}
noexec /tmp
