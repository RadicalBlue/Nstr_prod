#ifndef PIECE_H_
#define PIECE_H_

extern pthread_t robot_alim;

void * th_Piece(int code_piece, int num_machine);

#endif
