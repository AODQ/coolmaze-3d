#ifndef KEYS_H
#define KEYS_H

//Alphabetic Order to allow binary searching on string
#define KEYS_XMACRO \
X(0, XK_0, VK_0) \
X(1, XK_1, VK_1) \
X(2, XK_2, VK_2) \
X(3, XK_3, VK_3) \
X(4, XK_4, VK_4) \
X(5, XK_5, VK_5) \
X(6, XK_6, VK_6) \
X(7, XK_7, VK_7) \
X(8, XK_8, VK_8) \
X(9, XK_9, VK_9) \
X(down, XK_Down, VK_DOWN) \
X(left, XK_Left, VK_LEFT) \
X(lmouse, XK_Pointer_Button1, VK_LBUTTON) \
X(right, XK_Right, VK_RIGHT) \
X(rmouse, XK_Pointer_Button2, VK_RBUTTON) \
X(space, XK_space, VK_SPACE) \
X(up, XK_Up, VK_UP) \
X(A, XK_a, VK_A) \
X(B, XK_b, VK_B) \
X(C, XK_c, VK_C) \
X(D, XK_d, VK_D) \
X(E, XK_e, VK_E) \
X(F, XK_f, VK_F) \
X(G, XK_g, VK_G) \
X(H, XK_h, VK_H) \
X(I, XK_i, VK_I) \
X(J, XK_j, VK_J) \
X(K, XK_k, VK_K) \
X(L, XK_l, VK_L) \
X(M, XK_m, VK_M) \
X(N, XK_n, VK_N) \
X(O, XK_o, VK_O) \
X(P, XK_p, VK_P) \
X(Q, XK_q, VK_Q) \
X(R, XK_r, VK_R) \
X(S, XK_s, VK_S) \
X(T, XK_t, VK_T) \
X(U, XK_u, VK_U) \
X(V, XK_v, VK_V) \
X(W, XK_w, VK_W) \
X(X, XK_x, VK_X) \
X(Y, XK_y, VK_Y) \
X(Z, XK_z, VK_Z)

typedef enum {
  #define X(k, ...) KEY_##k ,
    KEYS_XMACRO
  #undef X
  NUM_KEYS,
  KEY_INVALID,
} keycode_t;

keycode_t keycode_from_string(const char * str);
void build_keysym_table(void);
int keysym_from_keycode(keycode_t k);
keycode_t keycode_from_keysym(int k);

#endif

