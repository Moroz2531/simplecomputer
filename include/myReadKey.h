#ifndef MYREADKEY_H
#define MYREADKEY_H

enum keys
{
  key_esape,
  key_enter,
  key_l,
  key_s,
  key_i,
  key_r,
  key_t,
  key_up,
  key_left,
  key_right,
  key_down,
  key_f5,
  key_f6,
  key_unknown,
};

int rk_readkey (enum keys *key);
int rk_mytermsave ();
int rk_mytermrestore ();
int rk_mytermregime (int regime, int vtime, int vmin, int echo, int sigint);
int rk_readvalue (int *value, int timeout);

#endif