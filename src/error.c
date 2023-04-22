
#include "../include/internal/error.h"

Error error_init()
{
	return (Error){
		.exists = FALSE};
}
