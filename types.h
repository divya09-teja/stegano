#ifndef TYPES_H                 // Prevents this header file from being included more than once
#define TYPES_H                 // Marks the beginning of the header file content

/* User defined types */
typedef unsigned int uint;      // Defines 'uint' as an alias for 'unsigned int'

/* Status will be used in fn. return type */
typedef enum                    // Start of enum definition for function return status
{
    e_failure,                  // Represents failure status (value = 0)
    e_success                   // Represents success status (value = 1)
} Status;                       // 'Status' becomes the name of this enum type

typedef enum                    // Start of enum for choosing operation type
{
    e_encode,                   // Represents encode operation
    e_decode,                   // Represents decode operation
    e_unsupported               // Represents unsupported/invalid operation
} OperationType;                // 'OperationType' becomes the name of this enum

#endif                          // End of TYPES_H header guard
