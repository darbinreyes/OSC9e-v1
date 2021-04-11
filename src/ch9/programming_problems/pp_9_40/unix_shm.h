#ifndef __UNIX_SHM_H__
#define __UNIX_SHM_H__
/*!
  @header Definitions shared by the producer and consumer.
*/

/*!
  @const shm_name
  @discussion File name used for the shared memory object.
*/
#define shm_name "collatz"

/*!
  @defined SHM_SIZE
  @discussion The size of the shared memory object.
*/
#define SHM_SIZE 4096

/*!
  @defined SHM_MODE
  @discussion The file mode associated with the shared memory object.
*/
#define SHM_MODE 0666

#endif