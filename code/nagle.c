When the application produces data to send 
    if both the available data and the window >= MSS 
        send a full segment 
    else 
        if there is unACKed data in flight 
            buffer the new data until an ACK arrives 
        else 
            send all the new data now 
