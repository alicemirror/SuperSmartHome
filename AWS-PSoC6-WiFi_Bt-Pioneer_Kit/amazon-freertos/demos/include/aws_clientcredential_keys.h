#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

#include <stdint.h>

/*
 * PEM-encoded client certificate.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----"
 */
#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWjCCAkKgAwIBAgIVAOEnGdMITept2Wt8zKGeGodxRsrMMA0GCSqGSIb3DQEB\n"\
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n"\
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMDA0MjkxNjMx\n"\
"MjhaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n"\
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDCGTJFZlhmSj0J70nB\n"\
"0QhwLT7Lb0Dr2H4ppZgypuS5cy4v/G7SXcxJ13SLLwQF8W98QBnYFCiBkrw159eh\n"\
"njxuObeou/vIiSbUbMjQ4OAFQtd9gOIO2GwFY6rOz8oeK5msj2z+YMHQqP5tQCjD\n"\
"4n0lqJRkzEF87JkQNjyajaewIAA9clPJrQR32nDXSsdZtB+DCc3qJPGrB6J7uru4\n"\
"zHm6ZhWXH5y+FtM1pbVqxOp14YIyBlPB6iM0ZxNk0LkN2G+5qXBiaajR1Utq2PRG\n"\
"9Zb02ntXDqeymJWgLpRu8yxo4xzQGDCsVAFyAHjaSauA8vIZ/AFooKV51oAYVDVO\n"\
"VSuhAgMBAAGjYDBeMB8GA1UdIwQYMBaAFM2BnxYsoMAQ367btdUOtFZoVM5uMB0G\n"\
"A1UdDgQWBBQ0zOTzwAG4LhWAItaUqKn0PNEY6DAMBgNVHRMBAf8EAjAAMA4GA1Ud\n"\
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAgdQYFNBO/DgWFqeuYx3r76jw\n"\
"25hM2TG56XeVRvNRNiIYPy4QGCPM68M1JG2FkE/y5Cd/HVA4+m4nFujtNN2TMR2z\n"\
"e2HRfNRWNBbtHAcJL5su11isQELT8dvh6lMXOyq2bKStZbiCgnICSAkuiSwITK3+\n"\
"XlAj3SBR1xbP9ezE0De2FLc9x6GdItHbwm8X7oZ1zmzjyEmoXMHaqgt0MXFIynzj\n"\
"/sp93M7avl9twzCsxEZy/KbsIrAEDhLaL83dVxU8Ony73Xqy+cNk7EcDdk2Fc65F\n"\
"mgP3yDssLGA1NvIvHgaSJrbPhjees34GOcpbSoFqzHaTVuL2KO9APitl8lYOmQ==\n"\
"-----END CERTIFICATE-----"

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----"
 */
#define keyCLIENT_PRIVATE_KEY_PEM \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEpQIBAAKCAQEAwhkyRWZYZko9Ce9JwdEIcC0+y29A69h+KaWYMqbkuXMuL/xu\n"\
"0l3MSdd0iy8EBfFvfEAZ2BQogZK8NefXoZ48bjm3qLv7yIkm1GzI0ODgBULXfYDi\n"\
"DthsBWOqzs/KHiuZrI9s/mDB0Kj+bUAow+J9JaiUZMxBfOyZEDY8mo2nsCAAPXJT\n"\
"ya0Ed9pw10rHWbQfgwnN6iTxqweie7q7uMx5umYVlx+cvhbTNaW1asTqdeGCMgZT\n"\
"weojNGcTZNC5DdhvualwYmmo0dVLatj0RvWW9Np7Vw6nspiVoC6UbvMsaOMc0Bgw\n"\
"rFQBcgB42kmrgPLyGfwBaKCledaAGFQ1TlUroQIDAQABAoIBAQCpTOj4d8jrkB2l\n"\
"qL6g+/9sOUroj2YFUZTaJsKWi9Q9niXPXVQc0gv67J+PN7c+p9Uif5cLTqn2O1NH\n"\
"ga4oHql/S4DTAQUhaeAgsbKH314HuMHVirGjWUr/eGvocH5PyqAEUCY1y5fCtWUX\n"\
"TjtAgTG/w0eMlE0a9hSce3JpLN3zVLAKilrtrJUuMxcn14zJCGijfsH1+lyka0Vd\n"\
"OkMO4iHDl4aRXe4Ls2x4nBvL5uCqoblwZ7/LH17k0SBgRY2oUadvwjTH5aPDM/J6\n"\
"rqtnYK6FzjOWyjQQxMA9CltvtolKmQEjzaxkN1+LzJh/lsuo0+sua4rziuiAVHS8\n"\
"3AtNg2ABAoGBAPMj2gL8KZ5wM+TDewtcSqWkMpd7Z2fQba9qBldXyF+6sqn5Tebo\n"\
"E++Eef0QaD4Tcm1V2WttjILZmyOvytV2SszoFPx5goEQSkspeKxOZWObStZdrwk6\n"\
"4YT2xClgz7j4K0DVqKziM6nvkhsxVVf3g8Dz5lav5P/MJ1TBBqR5IWPBAoGBAMxd\n"\
"UJDljiJASpwULO9PIKH/MebJU604agFcnrvKhTx02TnQNQ98L+MVNuDa5XmqPOfj\n"\
"UUnoo3GU/0C5GT6nu2x2kpZdnZrBj+ttooiKG+qXrnL9ykMvidjjsbYVx1X8UyIx\n"\
"78F5iP4pSyWXHafkggBAsRPAdpdHjkshoYr8cD/hAoGBALDto9WMPZTnvTt47G7l\n"\
"6AQFKjOFP0gSKd793kNsw4i1yRFbM08WDnu7ATIf4Ftbt2bMk/xSqW8GfphHcehH\n"\
"vkLBl5PpsAfPHOD4+rEYEzuaY6j+X0LuL/GbyD78Dexh2sfBMo8rw8CjbsSO77Wl\n"\
"jaC5WyI1uQRTQUOxeu3wn1WBAoGAC2dVnJBCqySdRTJXA4+NyZxc43zuf2b7e1H4\n"\
"xY/aeXInwPMeUtviLDC93DQ6w9w9A8+KdK5XmHnw+GKbxwRDEB1ZtqxIpB8S8dcv\n"\
"T6EtR5zpLh99TtxzO7gQ7ZEvi1vGI81XjJtEirIxMErLa9fzbSIu/2Qo+e4TQZwB\n"\
"0KifduECgYEAvs9LwjE4CyCIgHKWR4b2ia0x40bXx7/u6i0FXSj32pu4s9nLIOz0\n"\
"jXlsKxHCZb6C6lkWqi0Lw7kwL5QdBXnOWhx9sAgY/hxVtLrcqCYwDc74OUtYUkNj\n"\
"LPTyxCScIqy4L13jwLWevWFJZJAspJPESfomTmE/IkRPzP6n64EU//s=\n"\
"-----END RSA PRIVATE KEY-----"

/*
 * PEM-encoded Just-in-Time Registration (JITR) certificate (optional).
 *
 * If used, must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----"
 */
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  ""


#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
