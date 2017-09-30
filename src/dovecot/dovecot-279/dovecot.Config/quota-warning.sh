#!/bin/sh

USER="$1"
_quota_txt=/Applications/Server.app/Contents/ServerRoot/private/etc/mail/DefaultMessages.bundle/Contents/Resources/English.lproj/quota_warning.txt

if [ -e $_quota_txt ]; then
  cat $_quota_txt | /Applications/Server.app/Contents/ServerRoot/usr/libexec/dovecot/dovecot-lda -d "$USER" -o "plugin/quota=maildir:User quota:noenforcing"
fi
# Generate a Server.app alert
  echo "{eventType=com.apple.dovecot.quota.warning;userName=\"$USER\";}" | /usr/libexec/xssendevent
