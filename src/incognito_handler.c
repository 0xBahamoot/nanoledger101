
int incognito_handler()
    {
    int sw;

    if (((sw = check_potocol()) != SW_OK) || ((sw = check_ins_access() != SW_OK)))
        {
        monero_io_discard(0);
        return sw;
        }
    }
