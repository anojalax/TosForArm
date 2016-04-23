#include <kernel.h>

/*
 * get_mailbox_base
 * --------------
 *  return mailbox base address
 */
int get_mailbox_base() {
    return 0x2000B880;
}

/*
 * mailbox_write
 * ------------
 * Write to mailbox
 *
 * Parameters
 * content: message content, only use top 28 bits, lowest 4 bits should be 0
 * channel: mailbox using 4 bits.
 */
void mailbox_write(int content, int channel) {
    if (content & 0b1111) {   /* Make sure lowest 4 bits in r0 are all 0 */
        goto ERROR;

    }
    if (channel > 15)       /* Make sure mailbox is in 4 bits */
        goto ERROR;

    int *mailboxBase = (int *) get_mailbox_base();
    int status;

    /* Write wait */
    do {
        status = *(mailboxBase + 6);    /* Status address is 0x2000B898 */
    } while (status & 0x80000000);           /* Check top bit is 0 */

    content = content | channel;

    *(mailboxBase + 8) = content;           /* Send address is 0x2000B8A0 */
    ERROR:
    return;
}

/*
 * mailbox_read
 * -----------
 * Function to read from mailbox
 *
 * Parameters:
 * channel: mailbox, only using 4 bits.
 *
 * Return:
 * read message
 */
int mailbox_read(int channel) {
    if (channel > 15)       /* Make sure mailbox is in 4 bits */
        goto ERROR;

    int *mailboxBase = (int *) get_mailbox_base();
    int status, content, readChannel;

    while (1) {
        /* Write wait */
        do {
            status = *(mailboxBase + 6);    /* Status address is 0x2000B898 */
        } while (status & 0x80000000);           /* Check top bit is 0 */

        content = *(mailboxBase);
        readChannel = content & 0b1111;         /* Get channel information in msg */

        if (readChannel == channel) {    /* Make sure channel is match */
            //debug();
            break;
        }
    }
    return content & 0xFFFFFFF0;            /* Remove mail channel information in msg */
    ERROR:
    return 0;

}
