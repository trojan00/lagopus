/*
 * Copyright 2014-2015 Nippon Telegraph and Telephone Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <sys/queue.h>
#include "unity.h"
#include "../ofp_apis.h"
#include "handler_test_utils.h"
#include "event.h"
#include "lagopus/pbuf.h"
#include "lagopus/ofp_handler.h"
#include "../ofp_queue_handler.h"

void
setUp(void) {
}

void
tearDown(void) {
}

/* TODO remove */
static lagopus_result_t
ofp_multipart_request_handle_wrap(struct channel *channel,
                                  struct pbuf *pbuf,
                                  struct ofp_header *xid_header,
                                  struct ofp_error *error) {
  return ofp_multipart_request_handle(channel, pbuf, xid_header, error);
}

static struct queue_stats *
queue_stats_alloc(void) {
  struct queue_stats *queue_stats;
  queue_stats = (struct queue_stats *)
                calloc(1, sizeof(struct queue_stats));
  return queue_stats;
}

static void
create_data(struct queue_stats_list *queue_stats_list,
            int test_num) {
  struct queue_stats *queue_stats;
  int i;

  TAILQ_INIT(queue_stats_list);
  for (i = 0; i < test_num; i++) {
    queue_stats = queue_stats_alloc();

    queue_stats->ofp.port_no = (uint32_t) (0x01 + i);
    queue_stats->ofp.queue_id = (uint32_t) (0x02 + i);
    queue_stats->ofp.tx_bytes = (uint64_t) (0x03 + i);
    queue_stats->ofp.tx_packets = (uint64_t) (0x04 + i);
    queue_stats->ofp.tx_errors = (uint64_t) (0x05 + i);
    queue_stats->ofp.duration_sec = (uint32_t) (0x06 + i);
    queue_stats->ofp.duration_nsec = (uint32_t) (0x07 + i);

    TAILQ_INSERT_TAIL(queue_stats_list, queue_stats, entry);
  }
}

lagopus_result_t
ofp_queue_stats_reply_create_wrap(struct channel *channel,
                                  struct pbuf_list **pbuf_list,
                                  struct ofp_header *xid_header) {
  lagopus_result_t ret;
  struct queue_stats_list queue_stats_list;

  create_data(&queue_stats_list, 1);

  ret = ofp_queue_stats_reply_create(channel, pbuf_list,
                                     &queue_stats_list,
                                     xid_header);

  /* free. */
  queue_stats_list_elem_free(&queue_stats_list);

  return ret;
}

void
test_ofp_queue_stats_reply_create(void) {
  lagopus_result_t ret = LAGOPUS_RESULT_ANY_FAILURES;
  const char *data[1] = {"04 13 00 38 00 00 00 10"
                         "00 05 00 00 00 00 00 00"
                         "00 00 00 01 00 00 00 02"
                         "00 00 00 00 00 00 00 03"
                         "00 00 00 00 00 00 00 04"
                         "00 00 00 00 00 00 00 05"
                         "00 00 00 06 00 00 00 07"
                        };

  ret = check_pbuf_list_packet_create(ofp_queue_stats_reply_create_wrap,
                                      data, 1);

  TEST_ASSERT_EQUAL_MESSAGE(LAGOPUS_RESULT_OK, ret,
                            "ofp_queue_stats_reply_create(normal) error.");
}

lagopus_result_t
ofp_queue_stats_reply_create_null_wrap(struct channel *channel,
                                       struct pbuf_list **pbuf_list,
                                       struct ofp_header *xid_header) {
  lagopus_result_t ret;
  struct queue_stats_list queue_stats_list;

  ret = ofp_queue_stats_reply_create(NULL, pbuf_list,
                                     &queue_stats_list,
                                     xid_header);
  TEST_ASSERT_EQUAL_MESSAGE(LAGOPUS_RESULT_INVALID_ARGS, ret,
                            "ofp_queue_stats_request_create error.");

  ret = ofp_queue_stats_reply_create(channel, NULL,
                                     &queue_stats_list,
                                     xid_header);
  TEST_ASSERT_EQUAL_MESSAGE(LAGOPUS_RESULT_INVALID_ARGS, ret,
                            "ofp_queue_stats_request_create error.");

  ret = ofp_queue_stats_reply_create(channel, pbuf_list,
                                     NULL, xid_header);
  TEST_ASSERT_EQUAL_MESSAGE(LAGOPUS_RESULT_INVALID_ARGS, ret,
                            "ofp_queue_stats_request_create error.");

  ret = ofp_queue_stats_reply_create(channel, pbuf_list,
                                     &queue_stats_list,
                                     NULL);
  TEST_ASSERT_EQUAL_MESSAGE(LAGOPUS_RESULT_INVALID_ARGS, ret,
                            "ofp_queue_stats_request_create error.");

  /* Uncheck return value. */
  return -9999;
}

void
test_ofp_queue_stats_reply_create_null(void) {
  const char *data[1] = {"04 13 00 38 00 00 00 10"
                         "00 05 00 00 00 00 00 00"
                         "00 00 00 01 00 00 00 02"
                         "00 00 00 00 00 00 00 03"
                         "00 00 00 00 00 00 00 04"
                         "00 00 00 00 00 00 00 05"
                         "00 00 00 06 00 00 00 07"
                        };

  /* Uncheck return value. */
  (void) check_pbuf_list_packet_create(ofp_queue_stats_reply_create_null_wrap,
                                       data, 1);
}

void
test_ofp_queue_stats_request_handle_normal_pattern(void) {
  lagopus_result_t ret = LAGOPUS_RESULT_ANY_FAILURES;

  ret = check_packet_parse(ofp_multipart_request_handle_wrap,
                           "04 12 00 18 00 00 00 10"
                           "00 05 00 00 00 00 00 00"
                           "00 00 00 01 00 00 00 02");
  /* <---------> port_no : 1
   *             <---------> queue_id : 2
   */
  TEST_ASSERT_EQUAL_MESSAGE(LAGOPUS_RESULT_OK, ret,
                            "ofp_queue_stats_request_handle(normal) error.");
}

void
test_ofp_queue_stats_request_handle_error_invalid_length0(void) {
  lagopus_result_t ret = LAGOPUS_RESULT_ANY_FAILURES;
  struct ofp_error expected_error;
  ofp_error_set(&expected_error, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN);

  ret = check_packet_parse_expect_error(ofp_multipart_request_handle_wrap,
                                        "04 12 00 18 00 00 00 10"
                                        "00 05 00 00 00 00 00 00"
                                        "00 00 00 01 00 00 00",
                                        &expected_error);
  TEST_ASSERT_EQUAL_MESSAGE(LAGOPUS_RESULT_OFP_ERROR, ret,
                            "invalid-body error.");
}

void
test_ofp_queue_stats_request_handle_error_invalid_length1(void) {
  lagopus_result_t ret = LAGOPUS_RESULT_ANY_FAILURES;
  struct ofp_error expected_error;
  ofp_error_set(&expected_error, OFPET_BAD_REQUEST, OFPBRC_BAD_LEN);

  ret = check_packet_parse_expect_error(ofp_multipart_request_handle_wrap,
                                        "04 12 00 18 00 00 00 10"
                                        "00 05 00 00 00 00 00 00"
                                        "00 00 00 01 00 00 00 02"
                                        "00",
                                        &expected_error);
  TEST_ASSERT_EQUAL_MESSAGE(LAGOPUS_RESULT_OFP_ERROR, ret,
                            "invalid-body error.");
}

void
test_ofp_queue_stats_request_handle_null(void) {
  lagopus_result_t ret = LAGOPUS_RESULT_ANY_FAILURES;
  struct pbuf *pbuf = pbuf_alloc(65535);
  struct ofp_header xid_header;
  struct ofp_error error;
  struct event_manager *em = event_manager_alloc();
  struct channel *channel = channel_alloc_ip4addr("127.0.0.1", "1000",
                            em, 0x01);

  ret = ofp_queue_stats_request_handle(NULL, pbuf, &xid_header, &error);
  TEST_ASSERT_EQUAL_MESSAGE(LAGOPUS_RESULT_INVALID_ARGS, ret,
                            "ofp_queue_stats_request_handle error.");

  ret = ofp_queue_stats_request_handle(channel, NULL, &xid_header, &error);
  TEST_ASSERT_EQUAL_MESSAGE(LAGOPUS_RESULT_INVALID_ARGS, ret,
                            "ofp_queue_stats_request_handle error.");

  ret = ofp_queue_stats_request_handle(channel, pbuf, NULL, &error);
  TEST_ASSERT_EQUAL_MESSAGE(LAGOPUS_RESULT_INVALID_ARGS, ret,
                            "ofp_queue_stats_request_handle error.");

  ret = ofp_queue_stats_request_handle(channel, pbuf, &xid_header, NULL);
  TEST_ASSERT_EQUAL_MESSAGE(LAGOPUS_RESULT_INVALID_ARGS, ret,
                            "ofp_queue_stats_request_handle error.");

  /* after. */
  channel_free(channel);
  event_manager_free(em);
  pbuf_free(pbuf);
}
