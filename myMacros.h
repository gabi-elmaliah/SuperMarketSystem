#ifndef MYMACROS_H
#define MYMACROS_H
#define CHECK_RETURN_0(a) if (!(a)) return 0;
#define CHECK_MSG_RETURN_0(a) if(!(a)) {printf("error in pointer");\
										return;}
#define FREE_CLOSE_FILE_RETURN_0(pointer,file) free(pointer);\
								fclose((file));\
								return ;
#define CLOSE_RETURN_0(file) fclose((file));\
							return 0;





#endif // MYMACROS_H
