#include "string.h"

#include "stellar/parser.h"
#include "stellar/plugin.h"
#include "stellar/printer.h"

#define TOKEN_SYMBOL "TCOIN"
#define TOKEN_DECIMALS 7

void handle_init_contract(stellar_plugin_init_contract_t *msg) {
  // Make sure we are running a compatible version.
  if (msg->interface_version != STELLAR_PLUGIN_INTERFACE_VERSION_LATEST) {
    // If not the case, return the `UNAVAILABLE` status.
    msg->result = STELLAR_PLUGIN_RESULT_UNAVAILABLE;
    return;
  }

  msg->result = STELLAR_PLUGIN_RESULT_OK;
}

void handle_query_data_pair_count(stellar_plugin_query_data_pair_count_t *msg) {
  invoke_contract_args_t invoke_contract_args;
  if (msg->plugin_shared_ro->envelope->type ==
      ENVELOPE_TYPE_SOROBAN_AUTHORIZATION) {
    invoke_contract_args = msg->plugin_shared_ro->envelope
                               ->soroban_authorization.invoke_contract_args;
  } else {
    invoke_contract_args =
        msg->plugin_shared_ro->envelope->tx_details.tx.op_details
            .invoke_host_function_op.invoke_contract_args;
  }

  char function_name[SCV_SYMBOL_MAX_SIZE + 1] = {0};
  memcpy(function_name, invoke_contract_args.function.name,
         invoke_contract_args.function.name_size);

  // if the function name is `transfer`, we will return 3 data pairs.
  if (strcmp(function_name, "transfer") == 0) {
    msg->data_pair_count = 3;
    msg->result = STELLAR_PLUGIN_RESULT_OK;
    return;
  }

  // if the function name is `approve`, we will return 4 data pairs.
  if (strcmp(function_name, "approve") == 0) {
    msg->data_pair_count = 4;
    msg->result = STELLAR_PLUGIN_RESULT_OK;
    return;
  }

  // otherwise, we need to tell the caller that we can not handle this function.
  msg->result = STELLAR_PLUGIN_RESULT_UNAVAILABLE;
  return;
}

void handle_query_data_pair(stellar_plugin_query_data_pair_t *msg) {
  invoke_contract_args_t invoke_contract_args;
  if (msg->plugin_shared_ro->envelope->type ==
      ENVELOPE_TYPE_SOROBAN_AUTHORIZATION) {
    invoke_contract_args = msg->plugin_shared_ro->envelope
                               ->soroban_authorization.invoke_contract_args;
  } else {
    invoke_contract_args =
        msg->plugin_shared_ro->envelope->tx_details.tx.op_details
            .invoke_host_function_op.invoke_contract_args;
  }

  char function_name[SCV_SYMBOL_MAX_SIZE + 1] = {0};
  memcpy(function_name, invoke_contract_args.function.name,
         invoke_contract_args.function.name_size);
  buffer_t buffer = {.ptr = msg->plugin_shared_ro->raw,
                     .size = msg->plugin_shared_ro->raw_size,
                     .offset = invoke_contract_args.parameters_position};

  // Return the data pair.
  if (strcmp(function_name, "transfer") == 0) {
    switch (msg->data_pair_index) {
    case 0: {
      strlcpy(msg->caption, "Transfer", msg->caption_len);
      if (!read_scval_advance(&buffer) || !read_scval_advance(&buffer)) {
        msg->result = STELLAR_PLUGIN_RESULT_ERROR;
      }

      uint32_t sc_type;
      if (!parse_uint32(&buffer, &sc_type) || sc_type != SCV_I128 ||
          !print_int128(buffer.ptr + buffer.offset, TOKEN_DECIMALS, msg->value,
                        msg->value_len, true)) {
        msg->result = STELLAR_PLUGIN_RESULT_ERROR;
      }
      strlcat(msg->value, " ", msg->value_len);
      strlcat(msg->value, TOKEN_SYMBOL, msg->value_len);
      break;
    }
    case 1: {
      strlcpy(msg->caption, "From", msg->caption_len);
      sc_address_t from;
      uint32_t sc_type;

      if (!parse_uint32(&buffer, &sc_type) || sc_type != SCV_ADDRESS ||
          !parse_sc_address(&buffer, &from) ||
          !print_sc_address(&from, msg->value, msg->value_len, 0, 0)) {
        msg->result = STELLAR_PLUGIN_RESULT_ERROR;
      }

      break;
    }
    case 2:
      strlcpy(msg->caption, "To", msg->caption_len);
      if (!read_scval_advance(&buffer)) {
        msg->result = STELLAR_PLUGIN_RESULT_ERROR;
      }

      sc_address_t to;
      uint32_t sc_type;

      if (!parse_uint32(&buffer, &sc_type) || sc_type != SCV_ADDRESS ||
          !parse_sc_address(&buffer, &to) ||
          !print_sc_address(&to, msg->value, msg->value_len, 0, 0)) {
        msg->result = STELLAR_PLUGIN_RESULT_ERROR;
      }
      break;
    default:
      msg->result = STELLAR_PLUGIN_RESULT_ERROR;
    }
  }

  if (strcmp(function_name, "approve") == 0) {
    switch (msg->data_pair_index) {
    case 0: {
      strlcpy(msg->caption, "From", msg->caption_len);
      sc_address_t from;
      uint32_t sc_type;

      if (!parse_uint32(&buffer, &sc_type) || sc_type != SCV_ADDRESS ||
          !parse_sc_address(&buffer, &from) ||
          !print_sc_address(&from, msg->value, msg->value_len, 0, 0)) {
        msg->result = STELLAR_PLUGIN_RESULT_ERROR;
      }
      break;
    }
    case 1: {
      strlcpy(msg->caption, "Spender", msg->caption_len);
      if (!read_scval_advance(&buffer)) {
        msg->result = STELLAR_PLUGIN_RESULT_ERROR;
      }

      sc_address_t to;
      uint32_t sc_type;

      if (!parse_uint32(&buffer, &sc_type) || sc_type != SCV_ADDRESS ||
          !parse_sc_address(&buffer, &to) ||
          !print_sc_address(&to, msg->value, msg->value_len, 0, 0)) {
        msg->result = STELLAR_PLUGIN_RESULT_ERROR;
      }
      break;
    }
    case 2: {
      strlcpy(msg->caption, "Amount", msg->caption_len);
      if (!read_scval_advance(&buffer) || !read_scval_advance(&buffer)) {
        msg->result = STELLAR_PLUGIN_RESULT_ERROR;
      }

      uint32_t sc_type;
      if (!parse_uint32(&buffer, &sc_type) || sc_type != SCV_I128 ||
          !print_int128(buffer.ptr + buffer.offset, TOKEN_DECIMALS, msg->value,
                        msg->value_len, true)) {
        msg->result = STELLAR_PLUGIN_RESULT_ERROR;
      }

      strlcat(msg->value, " ", msg->value_len);
      strlcat(msg->value, TOKEN_SYMBOL, msg->value_len);
      break;
    }
    case 3: {
      strlcpy(msg->caption, "Live Until Ledger", msg->caption_len);
      if (!read_scval_advance(&buffer) || !read_scval_advance(&buffer) ||
          !read_scval_advance(&buffer)) {
        msg->result = STELLAR_PLUGIN_RESULT_ERROR;
      }

      uint32_t sc_type;
      if (!parse_uint32(&buffer, &sc_type) || sc_type != SCV_U32 ||
          !print_uint32(buffer.ptr + buffer.offset, 0, msg->value,
                        msg->value_len, false)) {
        msg->result = STELLAR_PLUGIN_RESULT_ERROR;
      }

      break;
    }
    default:
      msg->result = STELLAR_PLUGIN_RESULT_ERROR;
    }
    msg->result = STELLAR_PLUGIN_RESULT_OK;
  }

  msg->result = STELLAR_PLUGIN_RESULT_OK;
  return;
}