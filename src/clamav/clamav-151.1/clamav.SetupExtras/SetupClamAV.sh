
# Create config directory
_config_dir=/Library/Server/Mail/Config/clamav
if [ ! -d $_config_dir ]; then
  mkdir -p $_config_dir
fi

# Create data directory
_data_dir=/Library/Server/Mail/Data
if [ ! -d $_data_dir ]; then
  mkdir -p $_data_dir
fi

# Create clamav data directory
#  and set correct permissions and ownership
_clamav_dir=$_data_dir/scanner/clamav
if [ ! -d $_clamav_dir ]; then
  mkdir -p $_clamav_dir
fi

chown _clamav:_clamav $_clamav_dir
chmod 755 $_clamav_dir

# db dir
_clamav_db_dir=$_clamav_dir/db
if [ ! -d $_clamav_db_dir ]; then
  mkdir -p $_clamav_db_dir
fi

chown _clamav:_clamav $_clamav_db_dir
chmod 755 $_clamav_dir


# Install config files
_clamd_conf="$_config_dir/clamd.conf"
_clamd_conf_default="$_server_root/private/etc/clamd.conf.default"
if [ ! -e $_clamd_conf ]; then
  if [ -e $_clamd_conf_default ]; then
    cp $_clamd_conf_default $_clamd_conf
  fi
fi

chown root:wheel $_clamd_conf
chmod 644 $_clamd_conf

_freshclam_conf="$_config_dir/freshclam.conf"
_freshclam_conf_default="$_server_root/private/etc/freshclam.conf.default"
if [ ! -e $_freshclam_conf ]; then
  if [ -e $_freshclam_conf_default ]; then
    cp $_freshclam_conf_default $_freshclam_conf
  fi
fi

chown root:wheel $_freshclam_conf
chmod 644 $_freshclam_conf

# Setup log files
_clamav_log="/Library/Logs/Mail/clamav.log"
if [ ! -e $_clamav_log ]; then
  touch $_clamav_log
fi

# Set ownership and permissions
chown _amavisd:admin $_clamav_log
chmod 640 $_clamav_log

_freshclam_log="/Library/Logs/Mail/freshclam.log"
if [ ! -e $_freshclam_log ]; then
  touch $_freshclam_log
fi

# Set ownership and permissions
chown _clamav:admin $_freshclam_log
chmod 640 $_freshclam_log

# Set _clamav home to /var/clamav
`/usr/bin/dscl . -create /Users/_clamav NFSHomeDirectory /var/clamav`
`/usr/bin/dscl . -append /Groups/_amavisd GroupMembership _clamav`

