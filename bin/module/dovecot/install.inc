#!/bin/sh

hestia_module_dovecot_install() {
    source $HESTIA/bin/module/func.inc

    module_installed=$(hestia_module_isinstalled imap)
    if [ "$module_installed" ] && [ ! "$param_force" ]; then
        echo "IMAP module is already installed. See 'hestia module info imap'."
        exit 1
    fi

    echo "Installing IMAP (Dovecot) module..."

    osal_service_stop $OSAL_SERVICE_DOVECOT > /dev/null 2>&1
    hestia_config_backup 'dovecot-install' $OSAL_PATH_DOVECOT_CONF

    osal_package_preinstall
    osal_package_install $OSAL_PKG_DOVECOT

    gpasswd -a dovecot mail > /dev/null 2>&1
    rm -rf $OSAL_PATH_DOVECOT_CONF_D
    cp -rf $HESTIA_INSTALL_DIR/dovecot/* $OSAL_PATH_DOVECOT_CONF/
    cp -f $HESTIA_INSTALL_DIR/logrotate/dovecot $OSAL_PATH_LOGROTATE_CONF_D/
    chown -R root:root $OSAL_PATH_DOVECOT_CONF
    
    # FIXME: Why do we do this?
    rm -f /etc/dovecot/conf.d/15-mailboxes.conf

    osal_service_enable $OSAL_SERVICE_DOVECOT
    osal_service_start $OSAL_SERVICE_DOVECOT
    check_result $? "Dovecot start failed"

    osal_kv_write $HESTIA/conf/hestia.conf 'IMAP_SYSTEM' 'dovecot'
    osal_kv_write $HESTIA_CONF_MODULES/imap.conf 'installed' '1'
    osal_kv_write $HESTIA_CONF_MODULES/imap.conf 'description' 'Hestia IMAP (Dovecot) module'
    osal_kv_write $HESTIA_CONF_MODULES/imap.conf 'enabled' '1'
    osal_kv_write $HESTIA_CONF_MODULES/imap.conf 'variant' 'dovecot'
    osal_kv_write $HESTIA_CONF_MODULES/imap.conf 'version' '1'
    osal_kv_write $HESTIA_CONF_MODULES/imap.conf 'service_name' $OSAL_PKG_DOVECOT

    exit 0
}