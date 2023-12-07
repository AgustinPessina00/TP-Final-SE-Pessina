//=====[#include guards - begin]===============================================

#ifndef _GsmGprsCom_
#define _GsmGprsCom_

#include "arm_book_lib.h"
#include "mbed.h"
#include "nonBlockingDelay.h"
#include <string>

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================
typedef enum {
    GSM_GPRS_STATE_INIT,
    GSM_GPRS_STATE_IDLE,
    GSM_GPRS_STATE_AT_TO_BE_SEND,
    GSM_GPRS_STATE_AT_WAIT_FOR_RESPONSE,
    GSM_GPRS_STATE_ATPLUSCSQ_TO_BE_SEND,
    GSM_GPRS_STATE_ATPLUSCSQ_WAIT_FOR_RESPONSE,
    GSM_GPRS_STATE_ATPLUSCCID_TO_BE_SEND,
    GSM_GPRS_STATE_ATPLUSCCID_WAIT_FOR_RESPONSE,
    GSM_GPRS_STATE_ATPLUSCGREG_TO_BE_SEND,
    GSM_GPRS_STATE_ATPLUSCGREG_WAIT_FOR_RESPONSE, 
    GSM_GPRS_STATE_ATPLUSCGATT_TO_BE_SEND,
    GSM_GPRS_STATE_ATPLUSCGATT_WAIT_FOR_RESPONSE, 
    GSM_GPRS_STATE_ATPLUSCIPSHUT_TO_BE_SEND,
    GSM_GPRS_STATE_ATPLUSCIPSHUT_WAIT_FOR_RESPONSE,
    GSM_GPRS_STATE_ATPLUSCIPMUX_TO_BE_SEND,
    GSM_GPRS_STATE_ATPLUSCIPMUX_WAIT_FOR_RESPONSE,
    GSM_GPRS_STATE_ATPLUSCSTT_TO_BE_SEND,
    GSM_GPRS_STATE_ATPLUSCSTT_WAIT_FOR_RESPONSE,
    GSM_GPRS_STATE_ATPLUSCIICR_TO_BE_SEND,
    GSM_GPRS_STATE_ATPLUSCIICR_WAIT_FOR_RESPONSE,
    
    GSM_GPRS_STATE_ATPLUSCIFSR_TO_BE_SEND,
    GSM_GPRS_STATE_ATPLUSCIFSR_WAIT_FOR_RESPONSE,

    GSM_GPRS_STATE_ATPLUSCIPSTART_TO_BE_SEND,
    GSM_GPRS_STATE_ATPLUSCIPSTART_WAIT_FOR_RESPONSE,
    GSM_GPRS_STATE_CONNECTION_ESTABLISHED,

    GSM_GPRS_STATE_NO_SIM_CARD,
    GSM_GPRS_STATE_NO_SIGNAL,
    GSM_GPRS_STATE_DISCONNECTED,
} gsmGprsComState_t;


typedef enum {
    GSM_GPRS_STATE_NOT_READY_TO_SEND,
    GSM_GPRS_STATE_ATPLUSCIPSEND_TO_BE_SEND,
    GSM_GPRS_STATE_ATPLUSCIPSEND_WAIT_FOR_RESPONSE,
    GSM_GPRS_STATE_MESSAGE_READY_TO_BE_SEND,
    GSM_GPRS_STATE_MESSAGE_WAITING_FOR_CONFIRMATION,
    GSM_GPRS_STATE_MESSAGE_ALREADY_SENT,
} gsmGprsComSendStatus_t;

typedef enum {
    GSM_GPRS_STATE_DISCONNECTION_NOT_IN_PROCESS,
    GSM_GPRS_STATE_DISCONNECTION_ATPLUSCIPCLOSE_TO_BE_SEND,
    GSM_GPRS_STATE_DISCONNECTION_ATPLUSCIPCLOSE_WAIT_FOR_RESPONSE,
    GSM_GPRS_STATE_DISCONNECTION_ATPLUSCIPSHUT_TO_BE_SEND,
    GSM_GPRS_STATE_DISCONNECTION_ATPLUSCIPSHUT_WAIT_FOR_RESPONSE,
    GSM_GPRS_STATE_DISCONNECTION_SUCCESSFULL,
} gsmGprsComDisconnectionStatus_t;

//=====[Declarations (prototypes) of public functions]=========================

//=====[Declaration of public classes]=========================
     
/*
 * Class implementation for a driver beetween NUCLEO uc and GSM GPRS Module with SIM CARD
 * that uses UART interface
 * Sends, receives and interpret AT commands in order to connect, disconect and send messages through
 * TCP IP protocol
*/
class gsmGprsCom {

public:
//Public methods
    gsmGprsCom();
    gsmGprsCom( BufferedSerial * uartGsmGprs );
    void connect( );
    void send( const char * mensaje );
    void disconnect( );
    bool transmitionHasEnded( );
    void transmitionStart( );
    void transmitionStop( );
    bool disconnectionProcessHasEnded( );

private:
//Private attributtes
    gsmGprsComState_t gsmGprsComState;
    gsmGprsComSendStatus_t gsmGprsComSendStatus;
    gsmGprsComDisconnectionStatus_t gsmGprsComDisconnectionStatus;
    BufferedSerial* uartGsmGprs;
    nonBlockingDelay* refreshDelay;
    std::string localIP;
    float signalLevel;
    bool stopTransmition;

//Private methods
    void checkATPLUSCIPCLOSEcommand( );
    void sendAATPLUSCIPCLOSEcommand( );

    void checkmessageSendState( );
    void checkATPLUSCIPSENDcommand( );
    void sendATPLUSCIPSENDcommand( int strLen );

    void checkATPLUSCIPSTARTcommand( );
    void sendATPLUSCIPSTARTcommand( );
    void checkATPLUSCIFSRcommand( );
    void sendATPLUSCIFSRcommand( );
    void checkATPLUSCIICRcommand( );
    void sendATPLUSCIICRcommand( );
    void checkATPLUSCSTTcommand( );
    void sendATPLUSCSTTcommand( );
    void checkATPLUSCIPMUXcommand( );
    void sendATPLUSCIPMUXcommand( );
    void checkATPLUSCIPSHUTcommand( );
    void sendATPLUSCIPSHUTcommand( );
    void checkATPLUSCGATTcommand( );
    void sendATPLUSCGATTcommand( );
    void checkATPLUSCGREGcommand( );
    void sendATPLUSCGREGcommand( );
    void checkATPLUSCCIDcommand( );
    void sendATPLUSCCIDcommand( );
    void checkATPLUSCSQResponse( );
    void sendATPLUSCSQcommand( );
    void sendATCommand( ); 
    void checkATCommandResponse( );
    bool charRead( char* receivedChar );
    void write( const char* str );
    bool checkUARTResponse ( const char* stringToCheck );    
};

//=====[#include guards - end]=================================================

#endif /* _GsmGprsCom_ */
