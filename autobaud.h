/*! \file autobaud.h \brief Autobaud detection for the AVR */
#ifndef __AUTOBAUD_H__
#define __AUTOBAUD_H__
/*! \file autobaud.h
    \brief autobaud header file to autobaud AVR serial ports

*/

/*! \fn void autobaud_start(void)
    \brief Initializes the AHDLC engine.
*/
void 	autobaud_start(void); 

/*! \fn U8 autobaud_status(void)
    \brief Initializes the AHDLC engine.
*/
U8		autobaud_status(void);

/*! \fn U8 autobaud_rate(void)
    \brief Initializes the AHDLC engine.
*/
U8		autobaud_rate(void);

/*! \fn U8 autobaud_value(void)
    \brief Initializes the AHDLC engine.
*/
U8		autobaud_value(void);

/*! \fn U8 autobaud_setup(void)
    \brief Initializes the AHDLC engine.
*/
U8 		autobaud_setup(void);

#endif /* __AUTOBAUD_H__ */
