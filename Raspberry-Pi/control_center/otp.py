import string
import classes.secrets as secrets

alphabet = string.digits
password = ''.join(secrets.choice(alphabet) for i in range(4))

print (password)