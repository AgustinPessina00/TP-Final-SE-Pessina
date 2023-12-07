//=====[Libraries]=============================================================
#include "GsmGprsCom.h"
#include "arm_book_lib.h"
#include "mbed.h"
#include "string.h"
#include "nonBlockingDelay.h"

//=====[Declaration of private defines]========================================
#define REFRESH_TIME_10MS         10
#define REFRESH_TIME_1000MS       100
#define APN_USER_PASS "AT+CSTT=\"wap.gprs.unifon.com.ar \",\"wap\",\"wap\"\r\n"     //APN / username / password (CAMBIAR SI SE CAMBIA LA SIM!)  internet.gprs.unifon.com.ar
#define ATPLUSCIPSTART_IP_PORT "AT+CIPSTART=\"TCP\",\"181.94.69.108\",\"123\"\r\n"  //PROTOCOL / EXTERNAL IP / PORT 
                                                                                    //NOTA IP CAPITAL: 181.95.240.16
                                                                                    //NOTA IP LOBOS: 181.94.69.108
#define DEBUG
#define LOW_LEVEL_SIGNAL 6
#define CCID_VERIFICATION "8954078100795517486f" // (CAMBIAR SI SE CAMBIA LA SIM!)

//=====[Declaration of private functions]=====================================

static void pcSerialComCharWrite( char chr );

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

UnbufferedSerial uartUSB(USBTX, USBRX, 115200 ); //FOR DEBUG

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]=====================================

//-----------------------------------------------------------------------------
gsmGprsCom::gsmGprsCom() {
    this->refreshDelay =  new nonBlockingDelay ( REFRESH_TIME_1000MS  ); 
    this->uartGsmGprs = new BufferedSerial ( PE_8, PE_7, 9600 ); // (UART7_TX, UART7_RX, BAUDRATE)
    this->signalLevel = 0;
    this->gsmGprsComState = GSM_GPRS_STATE_INIT;
    this->gsmGprsComSendStatus = GSM_GPRS_STATE_NOT_READY_TO_SEND;
    this->gsmGprsComDisconnectionStatus = GSM_GPRS_STATE_DISCONNECTION_NOT_IN_PROCESS;
    this->stopTransmition = false;
}

//-----------------------------------------------------------------------------
gsmGprsCom::gsmGprsCom(BufferedSerial * serialCom) {
    this->uartGsmGprs = serialCom;
    this->gsmGprsComState = GSM_GPRS_STATE_INIT;
    this->gsmGprsComSendStatus = GSM_GPRS_STATE_NOT_READY_TO_SEND;
    this->gsmGprsComDisconnectionStatus = GSM_GPRS_STATE_DISCONNECTION_NOT_IN_PROCESS;
    this->stopTransmition = false;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::connect( ) {
    static int numberOfTries = 0;
    if( numberOfTries > 10 ) {
        this->gsmGprsComState = GSM_GPRS_STATE_INIT;  //if it can't send in n tries, stop trying to send and goes back to the first state
    }

    switch( this->gsmGprsComState ) {
        case GSM_GPRS_STATE_INIT: {
            numberOfTries = 0;
            if( this->stopTransmition == false ) {
                this->gsmGprsComState = GSM_GPRS_STATE_AT_TO_BE_SEND;
            }
        } break;

        case GSM_GPRS_STATE_AT_TO_BE_SEND: {
            this->sendATCommand( );
        } break;
            
        case GSM_GPRS_STATE_AT_WAIT_FOR_RESPONSE: {
            this->checkATCommandResponse( );
            if( this->refreshDelay->read( ) ) {
                this->gsmGprsComState = GSM_GPRS_STATE_DISCONNECTED;
            }
        } break;

        case  GSM_GPRS_STATE_ATPLUSCSQ_TO_BE_SEND: {
            this->sendATPLUSCSQcommand( );
        } break;

        case  GSM_GPRS_STATE_ATPLUSCSQ_WAIT_FOR_RESPONSE: {
            this->checkATPLUSCSQResponse( );
            if( this->refreshDelay->read( ) ) {
                this->gsmGprsComState = GSM_GPRS_STATE_NO_SIGNAL;
            }
        } break;

        case  GSM_GPRS_STATE_ATPLUSCCID_TO_BE_SEND: {
            this->sendATPLUSCCIDcommand( );
        } break;

        case  GSM_GPRS_STATE_ATPLUSCCID_WAIT_FOR_RESPONSE: {
            this->checkATPLUSCCIDcommand( );
            if( this->refreshDelay->read( ) ) {
                this->gsmGprsComState = GSM_GPRS_STATE_NO_SIM_CARD;
            }
        } break;

        case  GSM_GPRS_STATE_ATPLUSCGREG_TO_BE_SEND: {         //CREG SEND
            this->sendATPLUSCGREGcommand( );
        } break;

        case  GSM_GPRS_STATE_ATPLUSCGREG_WAIT_FOR_RESPONSE: {  //CREG WAIT
            this->checkATPLUSCGREGcommand( );
            if( this->refreshDelay->read( ) ) {
                this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCGREG_TO_BE_SEND;

                #ifdef DEBUG
                uartUSB.write( "\r\n", 3 );
                char msg[] = "ATPLUSCREG? NOT command responded correctly \r\n";
                numberOfTries++;
                uartUSB.write( msg, strlen(msg) );
                uartUSB.write( "\r\n", 3 );
                #endif
            }
        } break;

       case  GSM_GPRS_STATE_ATPLUSCGATT_TO_BE_SEND: { //CGATT SEND
            this->sendATPLUSCGATTcommand( );
        } break;

        case  GSM_GPRS_STATE_ATPLUSCGATT_WAIT_FOR_RESPONSE: {  //CGATT WAIT
            this->checkATPLUSCGATTcommand( );
            if( this->refreshDelay->read( ) ) {
                this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCGATT_TO_BE_SEND;
                numberOfTries++;

                #ifdef DEBUG
                uartUSB.write( "\r\n", 3 );
                char msg[] = "AT+GATT=1 NOT command responded correctly \r\n";
                uartUSB.write( msg, strlen (msg) );
                uartUSB.write( "\r\n", 3 );
                #endif
            }
        } break;

        case  GSM_GPRS_STATE_ATPLUSCIPSHUT_TO_BE_SEND: { //CIPSHUT SEND
            this->sendATPLUSCIPSHUTcommand( );
        } break;

        case  GSM_GPRS_STATE_ATPLUSCIPSHUT_WAIT_FOR_RESPONSE: {  //CIPSHUT WAIT
            this->checkATPLUSCIPSHUTcommand( );
            if( this->refreshDelay->read( ) ) {
                this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCIPSHUT_TO_BE_SEND;
                numberOfTries++;
              
                #ifdef DEBUG
                uartUSB.write( "\r\n", 3 );
                char msg[] = "AT+CIPSHUT NOT command responded correctly \r\n";
                uartUSB.write( msg, strlen (msg) );
                uartUSB.write( "\r\n", 3 );
                #endif
            }
        } break;

        case  GSM_GPRS_STATE_ATPLUSCIPMUX_TO_BE_SEND: { //CIPMUX SEND
            this->sendATPLUSCIPMUXcommand( );
        } break;

        case  GSM_GPRS_STATE_ATPLUSCIPMUX_WAIT_FOR_RESPONSE: {  //CIPMUX WAIT
            this->checkATPLUSCIPMUXcommand( );
            if( this->refreshDelay->read( ) ) {
                this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCIPMUX_TO_BE_SEND;
                numberOfTries++;
              
                #ifdef DEBUG
                uartUSB.write( "\r\n", 3 );
                char msg[] = "AT+CIPMUX=0  command responded NOT correctly \r\n";
                uartUSB.write( msg, strlen (msg) );
                uartUSB.write( "\r\n", 3 );
                #endif
            }
        } break;

        case  GSM_GPRS_STATE_ATPLUSCSTT_TO_BE_SEND: { //CSTT SEND
            this->sendATPLUSCSTTcommand( );
        } break;

        case  GSM_GPRS_STATE_ATPLUSCSTT_WAIT_FOR_RESPONSE: {  //CSTT WAIT
            this->checkATPLUSCSTTcommand( );
            if( this->refreshDelay->read( ) ) {
                numberOfTries++;
                this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCSTT_TO_BE_SEND;
                
                #ifdef DEBUG
                uartUSB.write( "\r\n", 3 );
                char msg[] = "AT+CSTT command responded NOT correctly \r\n";
                uartUSB.write(msg, strlen (msg));
                uartUSB.write( "\r\n", 3 );
                #endif
            }
        } break;

        case  GSM_GPRS_STATE_ATPLUSCIICR_TO_BE_SEND: { ////ATPLUSCIICR
            this->sendATPLUSCIICRcommand ();
        } break;

        case  GSM_GPRS_STATE_ATPLUSCIICR_WAIT_FOR_RESPONSE: {  ////ATPLUSCIICR
            this->checkATPLUSCIICRcommand( );
            if( this->refreshDelay->read( ) ) {
                this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCIICR_TO_BE_SEND;
                numberOfTries++;
              
                #ifdef DEBUG
                uartUSB.write( "\r\n", 3 );
                char msg[] = "AT+CIICR command responded NOT correctly \r\n";
                uartUSB.write(msg, strlen (msg));
                uartUSB.write( "\r\n", 3 );
                #endif
            }
        } break;

        case  GSM_GPRS_STATE_ATPLUSCIFSR_TO_BE_SEND: { //AT+CIFSR
            this->sendATPLUSCIFSRcommand( );
        } break;

        case  GSM_GPRS_STATE_ATPLUSCIFSR_WAIT_FOR_RESPONSE: {  //AT+CIFSR
            this->checkATPLUSCIFSRcommand( );
            if( this->refreshDelay->read( ) ) {
                this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCIFSR_TO_BE_SEND;
                numberOfTries++;

                #ifdef DEBUG
                uartUSB.write( "\r\n", 3 );
                char msg[] = "AT+CIFSR command responded NOT correctly \r\n";
                uartUSB.write(msg, strlen (msg));
                uartUSB.write( "\r\n", 3 );
                #endif
            }
        } break;

        case  GSM_GPRS_STATE_ATPLUSCIPSTART_TO_BE_SEND: { //AT+CIPSTART
            this->sendATPLUSCIPSTARTcommand( );
        } break;

        case  GSM_GPRS_STATE_ATPLUSCIPSTART_WAIT_FOR_RESPONSE: {  //AT+CIPSTART
            this->checkATPLUSCIPSTARTcommand();
            if( this->refreshDelay->read( ) ) {
               this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCIPSTART_TO_BE_SEND;
                numberOfTries++;

                #ifdef DEBUG
                uartUSB.write( "\r\n", 3 );
                char msg[] = "AT+CIPSTART command responded NOT correctly \r\n";
                uartUSB.write(msg, strlen (msg));
                uartUSB.write( "\r\n", 3 );
                #endif
            }
        } break;
        
        case  GSM_GPRS_STATE_NO_SIM_CARD: {
            numberOfTries++;
            this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCCID_TO_BE_SEND;

            #ifdef DEBUG
            uartUSB.write( "\r\n", 3 );
            char messageModuleWithoutSIMCard[] = "The Tracker cannot read the SIM CARD - If the error persist Please check if the Sim Card it's installed correctly\r\n";
            uartUSB.write(messageModuleWithoutSIMCard , strlen (messageModuleWithoutSIMCard) );
            uartUSB.write( "\r\n", 3 );
            #endif
        } break;

        case GSM_GPRS_STATE_DISCONNECTED: {
            numberOfTries++;
            this->gsmGprsComState = GSM_GPRS_STATE_AT_TO_BE_SEND;

            #ifdef DEBUG
            uartUSB.write( "\r\n",  3 );
            char messageModuleDisconected[] = "GSM GPRS Module disconnected or plug wrongly\r\n";
            uartUSB.write(messageModuleDisconected,  strlen (messageModuleDisconected) );
            uartUSB.write( "\r\n",  3 );
            #endif
        } break;

        case GSM_GPRS_STATE_NO_SIGNAL: {
            numberOfTries++;
            this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCSQ_TO_BE_SEND;

            #ifdef DEBUG
            uartUSB.write( "\r\n", 3 );
            char messageModuleWithNoSignal[] = "GSM GPRS Module without signal\r\n";
            uartUSB.write(messageModuleWithNoSignal,  strlen (messageModuleWithNoSignal) );
            uartUSB.write( "\r\n", 3 );
            #endif
        } break;

        case  GSM_GPRS_STATE_CONNECTION_ESTABLISHED: {
            numberOfTries = 0;
        } break;
    }
}

//-----------------------------------------------------------------------------
void gsmGprsCom::send( const char * message ) {
    static int numberTries = 0;
    char confirmationToSend[1];
    confirmationToSend[0] = '\x1a';

    if( this->gsmGprsComState != GSM_GPRS_STATE_CONNECTION_ESTABLISHED ) {
        return;
    }
    if( numberTries > 5 ) {
        this->gsmGprsComSendStatus = GSM_GPRS_STATE_MESSAGE_ALREADY_SENT;  //if it can't send in 5 five tries, stop trying to send and goes to disconnect
    }
    
    switch (this->gsmGprsComSendStatus) {
        case GSM_GPRS_STATE_NOT_READY_TO_SEND: {
            this->gsmGprsComSendStatus =  GSM_GPRS_STATE_ATPLUSCIPSEND_TO_BE_SEND;
        } break;

        case GSM_GPRS_STATE_ATPLUSCIPSEND_TO_BE_SEND: {
            this->sendATPLUSCIPSENDcommand( strlen(message) );
        } break;

        case GSM_GPRS_STATE_ATPLUSCIPSEND_WAIT_FOR_RESPONSE: {
                this->checkATPLUSCIPSENDcommand( ); 
                if( this->refreshDelay->read( ) ) { 
                    this->gsmGprsComSendStatus = GSM_GPRS_STATE_ATPLUSCIPSEND_TO_BE_SEND;   
                    numberTries++;

                    #ifdef DEBUG
                    uartUSB.write( "\r\n", 3 );
                    char msg[] = "AT+CIPSEND command responded NOT correctly \r\n";
                    uartUSB.write(msg, strlen (msg));
                    uartUSB.write( "\r\n", 3 );
                    #endif
            }
        } break;

       case  GSM_GPRS_STATE_MESSAGE_READY_TO_BE_SEND:  {
            this->write( message );
            this->gsmGprsComSendStatus = GSM_GPRS_STATE_MESSAGE_WAITING_FOR_CONFIRMATION; 
            this->refreshDelay->write( REFRESH_TIME_1000MS );
        } break;

        case GSM_GPRS_STATE_MESSAGE_WAITING_FOR_CONFIRMATION: {
            this->checkmessageSendState( ); 
                if( this->refreshDelay->read( ) ) { 
                    this->gsmGprsComSendStatus = GSM_GPRS_STATE_MESSAGE_READY_TO_BE_SEND;
                    numberTries++; 

                    #ifdef DEBUG
                    uartUSB.write( "\r\n",  3 );
                    char msg[] = "The message confirmation wasn't correctly recived\r\n";
                    uartUSB.write(msg, strlen (msg));
                    uartUSB.write( "\r\n",  3 );
                    #endif
            }
        } break;

        case  GSM_GPRS_STATE_MESSAGE_ALREADY_SENT:  {
            numberTries = 0;
        } break;
    }
}

//-----------------------------------------------------------------------------
bool gsmGprsCom::transmitionHasEnded( ) {
    if( this->gsmGprsComSendStatus == GSM_GPRS_STATE_MESSAGE_ALREADY_SENT )
        return true;
    else 
        return false;
}

//-----------------------------------------------------------------------------
bool gsmGprsCom::disconnectionProcessHasEnded( ) {
    if( this->gsmGprsComDisconnectionStatus == GSM_GPRS_STATE_DISCONNECTION_SUCCESSFULL )
        return true;
    else
        return false;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::transmitionStart( ) {
    this->gsmGprsComState = GSM_GPRS_STATE_INIT;
    this->gsmGprsComSendStatus = GSM_GPRS_STATE_NOT_READY_TO_SEND;
    this->gsmGprsComDisconnectionStatus = GSM_GPRS_STATE_DISCONNECTION_NOT_IN_PROCESS;
    this->stopTransmition = false;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::transmitionStop( ) {
    this->stopTransmition = true;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::disconnect( ) {    
    static int numberOfTries = 0;

    if (numberOfTries > 10) {
        this->gsmGprsComDisconnectionStatus = GSM_GPRS_STATE_DISCONNECTION_SUCCESSFULL;  //if it can't send in 10 tries, carry on with the rest of the rutine
    }

    switch (this->gsmGprsComDisconnectionStatus) {
        case GSM_GPRS_STATE_DISCONNECTION_NOT_IN_PROCESS: {
            numberOfTries = 0;
            this->gsmGprsComDisconnectionStatus =  GSM_GPRS_STATE_DISCONNECTION_ATPLUSCIPCLOSE_TO_BE_SEND;
        } break;

        case GSM_GPRS_STATE_DISCONNECTION_ATPLUSCIPCLOSE_TO_BE_SEND: {
            this->sendAATPLUSCIPCLOSEcommand( );
        } break;

        case GSM_GPRS_STATE_DISCONNECTION_ATPLUSCIPCLOSE_WAIT_FOR_RESPONSE: {
                this->checkATPLUSCIPCLOSEcommand( ); 
                if( this->refreshDelay->read( ) ) { 
                    numberOfTries++;
                    this->gsmGprsComDisconnectionStatus = GSM_GPRS_STATE_DISCONNECTION_ATPLUSCIPCLOSE_TO_BE_SEND;   
                  
                    #ifdef DEBUG
                    uartUSB.write( "\r\n", 3 );
                    char msg[] = "AT+CIPCLOSE command responded NOT correctly \r\n";
                    uartUSB.write(msg, strlen (msg));
                    uartUSB.write( "\r\n", 3 );
                    #endif
            }
        } break;

       case GSM_GPRS_STATE_DISCONNECTION_ATPLUSCIPSHUT_TO_BE_SEND: {
            this->write("AT+CIPSHUT\r\n"); // ATPLUSCIPSTART_IP_PORT
            this->refreshDelay->write( REFRESH_TIME_1000MS );
            this->gsmGprsComDisconnectionStatus =  GSM_GPRS_STATE_DISCONNECTION_ATPLUSCIPSHUT_WAIT_FOR_RESPONSE;
        } break;

        case GSM_GPRS_STATE_DISCONNECTION_ATPLUSCIPSHUT_WAIT_FOR_RESPONSE: {
                char expectedResponse[] = "SHUT OK";
                if (checkUARTResponse (expectedResponse )) {        
                    this->gsmGprsComDisconnectionStatus = GSM_GPRS_STATE_DISCONNECTION_SUCCESSFULL;

                    #ifdef DEBUG
                    uartUSB.write( "\r\n", 3 );
                    char msg[] = "Cip shut ok\r\n";
                    uartUSB.write( msg, strlen (msg) );
                    uartUSB.write( "\r\n", 3 );
                    #endif
                }
                if( this->refreshDelay->read( ) ) { 
                    numberOfTries++;
                    this->gsmGprsComDisconnectionStatus =  GSM_GPRS_STATE_DISCONNECTION_ATPLUSCIPSHUT_TO_BE_SEND;  

                    #ifdef DEBUG
                    uartUSB.write( "\r\n", 3 );
                    char msg[] = "AT+CIPSHUT command responded NOT correctly \r\n";
                    uartUSB.write(msg, strlen (msg));
                    uartUSB.write( "\r\n", 3 );
                    #endif
            }
        } break;

        case GSM_GPRS_STATE_DISCONNECTION_SUCCESSFULL:  {
            numberOfTries = 0;
            this->gsmGprsComDisconnectionStatus = GSM_GPRS_STATE_DISCONNECTION_NOT_IN_PROCESS; 
            this->gsmGprsComState = GSM_GPRS_STATE_INIT;
            this->gsmGprsComSendStatus = GSM_GPRS_STATE_NOT_READY_TO_SEND;

        } break;
    }
}

//-----------------------------------------------------------------------------
void gsmGprsCom::checkATPLUSCIPCLOSEcommand( ) {
    char expectedResponse[] = "OK";
    if( checkUARTResponse( expectedResponse ) ) {        
        this->gsmGprsComDisconnectionStatus = GSM_GPRS_STATE_DISCONNECTION_ATPLUSCIPSHUT_TO_BE_SEND;

        #ifdef DEBUG
        uartUSB.write ( "\r\n", 3 );
        char msg []  = "Cip close ok\r\n";
        uartUSB.write( msg, strlen (msg) );
        uartUSB.write ( "\r\n", 3 );
        #endif
    }
}

//-----------------------------------------------------------------------------
void gsmGprsCom::sendAATPLUSCIPCLOSEcommand( ) {
    this->write("AT+CIPCLOSE\r\n"); // ATPLUSCIPSTART_IP_PORT
    this->refreshDelay->write( REFRESH_TIME_1000MS );
    this->gsmGprsComDisconnectionStatus =  GSM_GPRS_STATE_DISCONNECTION_ATPLUSCIPCLOSE_WAIT_FOR_RESPONSE;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::checkmessageSendState( ) {
    char expectedResponse[] = "SEND OK";
    if( checkUARTResponse ( expectedResponse ) ) {        
        this->gsmGprsComSendStatus = GSM_GPRS_STATE_MESSAGE_ALREADY_SENT;

        #ifdef DEBUG
        uartUSB.write ( "\r\n", 3 );
        char msg[] = "message send!\r\n";
        uartUSB.write( msg, strlen (msg) );
        uartUSB.write ( "\r\n", 3 );
        #endif
    }
}

//-----------------------------------------------------------------------------
void gsmGprsCom::checkATPLUSCIPSENDcommand( ) {
    char receivedCharLocal;
    char expectedResponse  = '>';
    if( this->uartGsmGprs->readable( ) ) {
        this->uartGsmGprs->read( &receivedCharLocal, 1);

        #ifdef DEBUG
        pcSerialComCharWrite (receivedCharLocal);
        #endif

        if( receivedCharLocal == expectedResponse ) {
            this->gsmGprsComSendStatus = GSM_GPRS_STATE_MESSAGE_READY_TO_BE_SEND;
            
            #ifdef DEBUG
            uartUSB.write ( "\r\n", 3 );
            char msg[] = "ready to send\r\n";
            uartUSB.write( msg, strlen (msg) );
            uartUSB.write ( "\r\n", 3 );
            #endif
        }
    }
}

//-----------------------------------------------------------------------------
void gsmGprsCom::sendATPLUSCIPSENDcommand( int stringlen ) {
    char strToSend[50] = "";
    sprintf( strToSend, "AT+CIPSEND=%d\r\n", stringlen );
    this->write( strToSend );
    uartUSB.write( "\r\n ", 3 );  // debug on
    this->refreshDelay->write( REFRESH_TIME_1000MS );
    this->gsmGprsComSendStatus = GSM_GPRS_STATE_ATPLUSCIPSEND_WAIT_FOR_RESPONSE;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::checkATPLUSCIPSTARTcommand( ) {
    char expectedResponse[] = "ALREADY CONNECT";
    if( checkUARTResponse (expectedResponse ) ) {        
        this->gsmGprsComState = GSM_GPRS_STATE_CONNECTION_ESTABLISHED;
        
        #ifdef DEBUG
        uartUSB.write( "\r\n", 3 );
        char msg[] = "TCP connection established\r\n";
        uartUSB.write( msg, strlen (msg) );
        uartUSB.write( "\r\n", 3 );
        #endif
        
    }
}

//-----------------------------------------------------------------------------
void gsmGprsCom::sendATPLUSCIPSTARTcommand( )  {
    this->write(ATPLUSCIPSTART_IP_PORT); // ATPLUSCIPSTART_IP_PORT
    this->refreshDelay->write( REFRESH_TIME_1000MS );
    this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCIPSTART_WAIT_FOR_RESPONSE;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::checkATPLUSCIFSRcommand( ) {
    static bool firstIPNumberWasReceived = false;
    static std::string strNewIP = "";
    char receivedCharLocal = '\0';

    if( this->uartGsmGprs->readable( ) ) {
        this->uartGsmGprs->read( &receivedCharLocal, 1 );
        if( isdigit(receivedCharLocal) || receivedCharLocal == '.' ) {
            strNewIP += receivedCharLocal;    
            if( firstIPNumberWasReceived == false ) {
                firstIPNumberWasReceived = true;
            }
        }
        if ((receivedCharLocal == '\n' || receivedCharLocal ==  '\0') && firstIPNumberWasReceived == true ) {
            firstIPNumberWasReceived = false;
            this->localIP = strNewIP;

            #ifdef DEBUG
            uartUSB.write( "\r\n", 3 );
            char msg[] = "IP ADDRESS ASIGNATED:\r\n";
            uartUSB.write( msg, strlen (msg) );
            uartUSB.write( "\r\n", 3 );
            uartUSB.write( this->localIP.c_str(), this->localIP.length() );
            uartUSB.write( "\r\n", 3 );
            #endif

            strNewIP = "";
            this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCIPSTART_TO_BE_SEND;   //CAMBIAR ES SOLO A MODO DE PRUEBA
            return;
        }
    }
}

//-----------------------------------------------------------------------------
void gsmGprsCom::sendATPLUSCIFSRcommand( ) {
    this->write("AT+CIFSR\r\n "); // ATPLUSCIPSTART_IP_PORT
    this->refreshDelay->write( REFRESH_TIME_1000MS );
    this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCIFSR_WAIT_FOR_RESPONSE;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::checkATPLUSCIICRcommand( ) {
    char expectedResponse[] = "OK";
    if( checkUARTResponse (expectedResponse ) ) {
        this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCIFSR_TO_BE_SEND;

        #ifdef DEBUG
        uartUSB.write ( "\r\n",  3 );
        char msg[] = "connection to internet established\r\n";
        uartUSB.write( msg, strlen (msg) );
        uartUSB.write ( "\r\n",  3 );
        #endif
    }
}

//-----------------------------------------------------------------------------
void gsmGprsCom::sendATPLUSCIICRcommand( ) {
    this->write("AT+CIICR\r\n");
    this->refreshDelay->write( REFRESH_TIME_1000MS);
    this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCIICR_WAIT_FOR_RESPONSE;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::checkATPLUSCSTTcommand ()  {
    char expectedResponse[] = "OK";
    if(checkUARTResponse ( expectedResponse ) ) {
        this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCIICR_TO_BE_SEND;

        #ifdef DEBUG
        uartUSB.write( "\r\n", 3 );
        char msg[] = "apn, user name and password set up successfully\r\n";
        uartUSB.write( msg, strlen (msg) );
        uartUSB.write ( "\r\n",  3 );
        #endif
    }
}

//-----------------------------------------------------------------------------
void gsmGprsCom::sendATPLUSCSTTcommand( ) {
    uartUSB.write ("\r\n ", 3 );
    this->write(APN_USER_PASS);
    uartUSB.write ("\r\n ", 3 )
    this->refreshDelay->write( REFRESH_TIME_1000MS );
    this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCSTT_WAIT_FOR_RESPONSE;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::checkATPLUSCIPMUXcommand( ) {
    char expectedResponse [] = "OK";
    if (checkUARTResponse (expectedResponse )) {
        this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCSTT_TO_BE_SEND;

        #ifdef DEBUG
        uartUSB.write( "\r\n", 3 );
        char msg[] = "Configurated for one IP connection only \r\n";
        uartUSB.write( msg, strlen (msg) );
        uartUSB.write( "\r\n", 3 );
        #endif
    }
}

//-----------------------------------------------------------------------------
void gsmGprsCom::sendATPLUSCIPMUXcommand ()  {
    this->write( "AT+CIPMUX=0\r\n" );
    this->refreshDelay->write( REFRESH_TIME_1000MS );
    this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCIPMUX_WAIT_FOR_RESPONSE;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::checkATPLUSCIPSHUTcommand( )  {
    char expectedResponse[] = "SHUT OK";
    if (checkUARTResponse (expectedResponse )) {
        this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCIPMUX_TO_BE_SEND;

        #ifdef DEBUG
        uartUSB.write( "\r\n", 3 );
        char msg[] = "Last connection shut down \r\n";
        uartUSB.write( msg, strlen (msg) );
        uartUSB.write( "\r\n", 3 );
        #endif
    }
}

//-----------------------------------------------------------------------------
void gsmGprsCom::sendATPLUSCIPSHUTcommand( ) {
    this->write( "AT+CIPSHUT\r\n");
    this->refreshDelay->write( REFRESH_TIME_1000MS );
    this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCIPSHUT_WAIT_FOR_RESPONSE;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::checkATPLUSCGATTcommand ()  {
    char expectedResponse[] = "OK";
    if( checkUARTResponse( expectedResponse ) ) {
        this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCIPSHUT_TO_BE_SEND;

        #ifdef DEBUG
        uartUSB.write ( "\r\n", 3 );
        char msg[] = "Device attach to the GPRS Network \r\n";
        uartUSB.write( msg, strlen (msg) );
        uartUSB.write ( "\r\n", 3 );
        #endif
    }  
}

//-----------------------------------------------------------------------------
void gsmGprsCom::sendATPLUSCGATTcommand( ) {
    this->write( "AT+CGATT=1\r\n");
    this->refreshDelay->write( REFRESH_TIME_1000MS );
    this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCGATT_WAIT_FOR_RESPONSE;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::checkATPLUSCGREGcommand( )  {
    char expectedResponse[] = "OK";
    if( checkUARTResponse ( expectedResponse )) {
        this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCGATT_TO_BE_SEND ;
        
        #ifdef DEBUG
        uartUSB.write ( "\r\n", 3 );
        char msg[] = "Device Registed and operational \r\n";
        uartUSB.write( msg, strlen(msg) );
        uartUSB.write ( "\r\n", 3 );
        #endif
    }
}

//-----------------------------------------------------------------------------
void gsmGprsCom::sendATPLUSCGREGcommand( ) {
    this->write( "AT+CREG?\r\n" );
    this->refreshDelay->write( REFRESH_TIME_1000MS );
    this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCGREG_WAIT_FOR_RESPONSE;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::checkATPLUSCCIDcommand () {
    if( checkUARTResponse( CCID_VERIFICATION ) ) {
        this->gsmGprsComState =  GSM_GPRS_STATE_ATPLUSCGREG_TO_BE_SEND;

        #ifdef DEBUG
        uartUSB.write ( "\r\n", 3 );
        char msg []  = "CCID command responded correctly \r\n";
        uartUSB.write( msg, strlen (msg) );
        uartUSB.write ( "\r\n", 3 );
        #endif
    } 
}

//-----------------------------------------------------------------------------
void gsmGprsCom::sendATPLUSCCIDcommand( )  {
    this->write( "AT+CCID\r\n");
    this->refreshDelay->write( REFRESH_TIME_1000MS );
    this->gsmGprsComState =  GSM_GPRS_STATE_ATPLUSCCID_WAIT_FOR_RESPONSE;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::checkATPLUSCSQResponse( ) {
    char stringToCheck [7] = "+CSQ: ";
    char msgStringSignalQuality [9]= "";
    static std::string strSignalQuality = "";
    int stringIndexSignalQuality = 5;
    static int responseStringPositionIndex = 0;
    char charReceived;
    char charSignalLevel;

    if( this->charRead( &charReceived ) ) {
        pcSerialComCharWrite( charReceived );
        if (charReceived == stringToCheck[responseStringPositionIndex] && responseStringPositionIndex < (strlen (stringToCheck) ))  { // && responseStringPositionIndex <= strlen (stringToCheck) -1
            responseStringPositionIndex++;
        }
        else if( charReceived != stringToCheck[responseStringPositionIndex] && responseStringPositionIndex < (strlen(stringToCheck)) )  { 
            responseStringPositionIndex = 0;
            strSignalQuality = ""; //
        }
        else if( (responseStringPositionIndex >= (strlen(stringToCheck) - 1 )) ) {
            if( isdigit(charReceived) ) {
                strSignalQuality += charReceived;
            } 
            responseStringPositionIndex++;
            if( (responseStringPositionIndex == (strlen (stringToCheck) + 2 )) ) {       
                this->signalLevel = std::stof(strSignalQuality);

                #ifdef DEBUG     
                sprintf (msgStringSignalQuality, "%f", this->signalLevel );  
                uartUSB.write ( "\r\n",  3 );
                char msg3 []  = "signal level: ";
                uartUSB.write (msg3,  strlen (msg3) );
                uartUSB.write (msgStringSignalQuality,  strlen (msgStringSignalQuality) );
                #endif

                if (this->signalLevel < LOW_LEVEL_SIGNAL) {
                    this->gsmGprsComState = GSM_GPRS_STATE_NO_SIGNAL;
                    responseStringPositionIndex = 0;
                    strSignalQuality = "";
                    return;
                } else {
                    this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCCID_TO_BE_SEND;
                    responseStringPositionIndex = 0;
                    strSignalQuality = "";
                    return;
                }
            }  
        } 
    }    
}

//-----------------------------------------------------------------------------
 void gsmGprsCom::sendATPLUSCSQcommand( ) {
    this->write( "AT+CSQ\r\n");
    this->refreshDelay->write( REFRESH_TIME_1000MS );
    this->gsmGprsComState =   GSM_GPRS_STATE_ATPLUSCSQ_WAIT_FOR_RESPONSE;
}

//-----------------------------------------------------------------------------
 void gsmGprsCom::sendATCommand( ) {
    this->write( "AT\r\n");
    this->refreshDelay->write( REFRESH_TIME_1000MS );
    this->gsmGprsComState = GSM_GPRS_STATE_AT_WAIT_FOR_RESPONSE;
}

//-----------------------------------------------------------------------------
void gsmGprsCom::checkATCommandResponse( ) {
    char expectedResponse[] = "OK";
    if( checkUARTResponse( expectedResponse ) ) {
        this->gsmGprsComState = GSM_GPRS_STATE_ATPLUSCSQ_TO_BE_SEND;

        #ifdef DEBUG
        uartUSB.write( "\r\n", 3 );
        char msg[] = "AT command responded correctly \r\n";
        uartUSB.write( msg, strlen(msg) );
        uartUSB.write( "\r\n", 3 );
        #endif
    } 
}

//-----------------------------------------------------------------------------
void gsmGprsCom::write( const char* str ) {
    this->uartGsmGprs->write( str, strlen(str) );
}

//-----------------------------------------------------------------------------
bool gsmGprsCom::charRead( char* receivedChar ) {
    char receivedCharLocal = '\0';
    if( this->uartGsmGprs->readable( ) ) {
        this->uartGsmGprs->read( &receivedCharLocal , 1);
        *receivedChar = receivedCharLocal;       
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
bool gsmGprsCom::checkUARTResponse(const char* stringToCheck) {
    static int responseStringPositionIndex = 0;
    char charReceived;
    bool moduleResponse = false;

    if( this->charRead( &charReceived ) ) {
        if( charReceived == stringToCheck[responseStringPositionIndex] ) {
            responseStringPositionIndex++;
            if( stringToCheck[responseStringPositionIndex] == '\0' ) {
                responseStringPositionIndex = 0;
                moduleResponse = true;
            }
        } 
        else {
            responseStringPositionIndex = 0;
        }
    }
    return moduleResponse;
}

//-----------------------------------------------------------------------------
static void pcSerialComCharWrite( char chr ) {
    char str[2] = "";
    sprintf (str, "%c", chr);
    uartUSB.write( str, strlen(str) );
}