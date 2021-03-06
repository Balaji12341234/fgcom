
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static int s_index;
static int s_file_handle;
static char *s_content;
static int s_size;

/**
 *
 * \fn void parser_init(void)
 *
 * \brief Starts parser initialization.
 *
 */
int parser_init(const char *filename)
{
	struct stat l_stat;
	ssize_t l_nbytes;
	int l_status;

	s_index = 0;

	if((s_file_handle = open(filename, O_RDONLY)) < 0)
		return (s_file_handle);

	fstat(s_file_handle, &l_stat);

	l_status = -1;
	if((s_content = (char *)malloc((l_stat.st_size + 1) * sizeof(char))) != NULL)
	{
		if((l_nbytes = read(s_file_handle, s_content, l_stat.st_size)) == l_stat.st_size)
		{
			l_status = 0;
			s_size = l_stat.st_size;
		}
	}
	close(s_file_handle);

	return(l_status);
}

/**
 *
 * \fn void parser_exit(void)
 *
 * \brief Exits parser.
 *
 */
void parser_exit(void)
{
	free(s_content);
}

/**
 *
 * \fn int parser_get_next_value(const char *line, float *value)
 *
 * \brief Extract a numeric value.
 *
 * \param line	pointer on the line extracted from the input file.
 * \param value	pointer on the returned value.
 *
 * \return Returns 0 if value successfully extracted. Otherwhise, returns
 * a negative value meaning that an error occured.
 *
 */
int parser_get_next_value(double *value)
{
	int l_status = 0;
	unsigned int l_j;
	unsigned int l_size;
	char *l_buf;

	/* Check if we are already at the end of the string. */
	if(s_index >= s_size)
		return(-1);

	/* Enter main parser loop. */
	while((s_index < s_size) && (l_status == 0))
	{
		/* Search for something different than an espace or tab. */
		while(  (s_content[s_index] == ' ' || s_content[s_index] == '\t') &&
				(s_index < s_size) )
			s_index++;

		/* If we have reached end of file, we exit now. */
		if (s_index >= s_size)
			return(-1);

		/* If character is a CR, we restart for next line. */
		if(s_content[s_index] == '\n')
		{
			s_index++;
			continue;
		}

		/* Is it a comment ? */
		if(s_content[s_index] == '#')
		{
			/* Yes, go until end of line. */
			while((s_content[s_index] != '\n') && (s_index < s_size))
				s_index++;
		}
		else
		{
			/* We have found something that is not a comment. */
			while((s_content[s_index] < '0' || s_content[s_index] > '9') && (s_index < s_size))
				s_index++;

			if(s_index < s_size)
			{
				l_j = s_index + 1;
				while(  ((s_content[l_j] >= '0' && s_content[l_j] <= '9') ||
						 (s_content[l_j] == '.' || s_content[l_j] == ',')) &&
						((s_content[l_j] != '\n') && (l_j < s_size)) )
					l_j++;

				l_size = l_j - s_index + 1;
				if((l_buf = (char *)malloc(l_size * sizeof(char))) != NULL)
				{
					/* Initialize buffer with O. */
					memset((void *)l_buf, 0, l_size);
					bcopy((const void *)(s_content + s_index), (void *)l_buf, l_size - 1);
					/* Convert string into double. */
					*value = atof(l_buf);
					/* Buffer is not needed any longer. */
					free(l_buf);
					/* Prepare for next value. */
					s_index = l_j + 1;
					break;
				}
			}
		}
	} /* while((s_index < s_size) && (l_status == 0)) */

	return(0);
}




