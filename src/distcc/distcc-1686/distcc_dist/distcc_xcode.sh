#!/bin/sh

# distcc -- A simple distributed compiler system
#
# Copyright 2009 Google Inc.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
# USA.

# Authors: Mark Mentovai, Thomas Van Lenten

# This is a temporary integration shim to allow xcodebuild-driven builds
# to use distcc pump mode.  Because pump mode is enabled in part by the
# DISTCC_HOSTS environment variable, and xcodebuild resets that variable,
# this script is used to catch what Xcode does and re-reset the variable
# when pump mode is desired.

# If no include server, just enable lzo.
DISTCC_MODES="lzo"
NEW_DISTCC_HOSTS=""
if [ -n "${INCLUDE_SERVER_PORT}" ] ; then
  DISTCC_MODES="lzo,cpp"
  # Add in --randomize to also distribute things to a different host each time.
  NEW_DISTCC_HOSTS="--randomize"
fi

for host in ${DISTCC_HOSTS} ; do
  if [ "${host}" = "localhost" ] ; then
    # Don't rewrite localhost.  When distcc finds localhost by itself
    # without anything appended, it just runs the compilation locally
    # without trying to connect to a local distccd.
    NEW_DISTCC_HOSTS="${NEW_DISTCC_HOSTS} ${host}"
  else
    NEW_DISTCC_HOSTS="${NEW_DISTCC_HOSTS} ${host},${DISTCC_MODES}"
  fi
done

export DISTCC_HOSTS="${NEW_DISTCC_HOSTS}"

exec "${0}_real" "${@}"

exit 1
