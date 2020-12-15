
#ifndef INCOGNITO_API_H
#define INCOGNITO_API_H

void incognito_install();
void incognito_init(void);
void incognito_init_private_key(void);
void incognito_wipe_private_key(void);

void incognito_init_ux(void);
int incognito_dispatch(void);