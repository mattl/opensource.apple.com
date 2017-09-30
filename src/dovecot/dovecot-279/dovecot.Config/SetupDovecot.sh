
################
# Do nothing on re-promotion

server_inited=`/usr/libexec/PlistBuddy -c "print service_initialized" /Library/Server/Mail/Config/MailServicesOther.plist`
if [ "$server_inited" == "true" ]; then
  exit 0
fi

################
# Mail directories

_dovecot_db_dir=/Library/Server/Mail/Data/db
_dovecot_mail_dir=/Library/Server/Mail/Data/mail
_dovecot_sieve_dir=/Library/Server/Mail/Data/rules
_dovecot_config_dir=/Library/Server/Mail/Config
_mail_logs_path=/Library/Logs/Mail
_mailaccess_log=/Library/Logs/Mail/mailaccess.log

################
# Move config files into /Library/Server
if [ ! -d $_dovecot_config_dir ]; then
  mkdir -p $_dovecot_config_dir
fi

if [ -d $_server_root/private/etc/dovecot ]; then
  cp -rpf "$_server_root/private/etc/dovecot" "$_dovecot_config_dir/"
fi

################
# Create dovecot.conf file on clean install

(
	cd $_dovecot_config_dir/dovecot/default
	find -s * -type f -print | while read f
	do
		if [ ! -e ../"$f" ]
		then
			cp -v "$f" ../"$f"
			chmod 644 ../"$f"
		fi
	done
)

hostname=`grep "^myhostname *=" /Library/Server/Mail/Config/postfix/main.cf | sed 's,.*= *,,'`
if [ ! "$hostname" ] ; then
  hostname=`hostname`
fi
if [ "$hostname" ] ; then
  perl -p -i -e '
	s/^(\s*)#\s*postmaster_address\s*=.*/$1postmaster_address = postmaster\@'"$hostname"'/;
	s/^(\s*)postmaster_address\s*=\s*postmaster\@example\.com/$1postmaster_address = postmaster\@'"$hostname"'/;
  ' $_dovecot_config_dir/dovecot/*.conf $_dovecot_config_dir/dovecot/conf.d/*.conf
fi

# Create submit.passdb with either the same password postfix is configured for,
# or an unguessable random password on clean install.
if [ ! -e $_dovecot_config_dir/dovecot/submit.passdb ] ; then
  if [ "$hostname" -a -s /Library/Server/Mail/Config/postfix/submit.cred ] ; then
    pw=`grep "^$hostname|submit|" /Library/Server/Mail/Config/postfix/submit.cred | sed 's,.*|,,'`
  fi
  if [ ! "$pw" ] ; then
    pw=`dd if=/dev/urandom bs=256 count=1 | env LANG=C tr -dc a-zA-Z0-9 | cut -b 1-22`
  fi
  if [ "$pw" ] ; then
    echo "submit:{PLAIN}$pw" > $_dovecot_config_dir/dovecot/submit.passdb
    chown :mail $_dovecot_config_dir/dovecot/submit.passdb
    chmod 640 $_dovecot_config_dir/dovecot/submit.passdb
  fi
fi

###############
# Verify dovecot default directories

if [ ! -d $_dovecot_db_dir ]; then
  mkdir -p $_dovecot_db_dir
fi

if [ ! -d $_dovecot_mail_dir ]; then
  mkdir -p $_dovecot_mail_dir
fi

if [ -d $_dovecot_mail_dir ]; then
  chmod 775 $_dovecot_mail_dir
  chown _dovecot:mail $_dovecot_mail_dir
fi

if [ ! -d $_dovecot_sieve_dir ]; then
  mkdir -p $_dovecot_sieve_dir
fi

if [ -d $_dovecot_sieve_dir ]; then
  chmod 775 $_dovecot_sieve_dir
  chown _dovecot:mail $_dovecot_sieve_dir
fi

if [ ! -d $_mail_logs_path ]; then
  mkdir -p $_mail_logs_path
fi

if [ ! -e $_mailaccess_log ]; then
  touch $_mailaccess_log
fi

if [ -e $_mailaccess_log ]; then
  chmod 640 $_mailaccess_log
  chown _dovecot:admin $_mailaccess_log
fi

if [ ! -d /var/db/dovecot.fts.update ]; then
  mkdir -p -m 770 /var/db/dovecot.fts.update
  chown _dovecot:mail /var/db/dovecot.fts.update
fi

if ! grep '^local6\.' /etc/syslog.conf >/dev/null
then
  echo "local6.warn\t\t\t\t\t\t$_mailaccess_log" >> /etc/syslog.conf
  kill -1 `cat /var/run/syslog.pid`
fi
