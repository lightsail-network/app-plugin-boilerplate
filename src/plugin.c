#include "string.h"
#include "stellar/plugin.h"
#include "stellar/printer.h"

void handle_init_contract(stellar_plugin_init_contract_t *msg)
{
    // Make sure we are running a compatible version.
    if (msg->interface_version != STELLAR_PLUGIN_INTERFACE_VERSION_LATEST)
    {
        // If not the case, return the `UNAVAILABLE` status.
        msg->result = STELLAR_PLUGIN_RESULT_UNAVAILABLE;
        return;
    }

    msg->result = STELLAR_PLUGIN_RESULT_OK;
}

void handle_query_data_pair_count(stellar_plugin_query_data_pair_count_t *msg)
{
    invoke_contract_args_t invoke_contract_args;
    if (msg->plugin_shared_ro->envelope->type == ENVELOPE_TYPE_SOROBAN_AUTHORIZATION)
    {
        invoke_contract_args = msg->plugin_shared_ro->envelope->soroban_authorization.invoke_contract_args;
    }
    else
    {
        invoke_contract_args = msg->plugin_shared_ro->envelope->tx_details.tx.op_details.invoke_host_function_op.invoke_contract_args;
    }

    char function_name[33] = {0};
    memcpy(function_name, invoke_contract_args.function.name, invoke_contract_args.function.name_size);
    if (strcmp(function_name, "testfunc") != 0)
    {
        msg->result = STELLAR_PLUGIN_RESULT_UNAVAILABLE;
        return;
    }

    // Return the number of data pairs.
    msg->data_pair_count = 4;
    msg->result = STELLAR_PLUGIN_RESULT_OK;
}

void handle_query_data_pair(stellar_plugin_query_data_pair_t *msg)
{
    // Return the data pair.
    switch (msg->data_pair_index)
    {
    case 0:
        strlcpy(msg->caption, "Caption 1", msg->caption_len);
        print_int64_num(1000, msg->value,  msg->value_len);
        break;
    case 1:
        strlcpy(msg->caption, "Caption 2", msg->caption_len);
        strlcpy(msg->value, "Value 2", msg->value_len);
        break;
    case 2:
        strlcpy(msg->caption, "Caption 3", msg->caption_len);
        strlcpy(msg->value, "Value 3", msg->value_len);
        break;
    case 3:
        strlcpy(msg->caption, "Caption 4", msg->caption_len);
        strlcpy(msg->value, "Value 4", msg->value_len);
        break;
    default:
        msg->result = STELLAR_PLUGIN_RESULT_ERROR;
        return;
    }

    msg->result = STELLAR_PLUGIN_RESULT_OK;
}