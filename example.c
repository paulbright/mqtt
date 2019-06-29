/*
 * Copyright 2018 Thingstream AG
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "client_api.h"
#include "line_buffer_transport.h"
#include "modem_transport.h"
#include "base64_codec_transport.h"
#include "thingstream_transport.h"
#include "client_platform.h"
#include "log_client_transport.h"
#include "log_modem_transport.h"

#include "serial_transport.h"

#define THINGSTREAM_BUFFER_LENGTH 512
static uint8_t thingstream_buffer[THINGSTREAM_BUFFER_LENGTH];

#define RX_BUFFER_LENGTH 300
static uint8_t rx_buffer[RX_BUFFER_LENGTH];

#define EXAMPLE_TOPIC "test/paul"
static uint16_t exampleTopicId = 0; /* will get updated after registration */

static bool done = false;

/* Callback for receiving messages.
 * This will only ever be called from within Client.run()
 */
static void receiveCallback(void *cookie, Topic topic, QOS qos, uint8_t* msg, uint16_t msgLen)
{
    if (msgLen > 0)
    {
        printf("Received message: '%.*s'\n", msgLen, msg);
    }
    else
    {
        puts("Received empty message");
    }

    if (topic.topicId == exampleTopicId)
    {
        /* received the expected message */
        done = true;
    }
}

#define CHECK(msg, cond) do { \
        printf("%s %s @%d\n", msg, ((cond) ? "ok" : "ERROR"), __LINE__); \
        if (!(cond)) goto error; \
    } while(0)

int main (int argc, char *argv[])
{
    ClientResult cr;
    char *serial = argc > 1 ? argv[1] : "/dev/ttyUSB0";
    Transport* transport = serial_transport_create(serial);
    CHECK("serial", transport != NULL);

    transport = line_buffer_transport_create(transport, rx_buffer, RX_BUFFER_LENGTH);
    CHECK("linebuf", transport != NULL);

#if (defined(DEBUG_LOG_MODEM) && (DEBUG_LOG_MODEM > 0))
    transport = log_modem_transport_create(transport, printf, 0xFF);
    CHECK("log_modem", transport != NULL);
#endif /* DEBUG_LOG_MODEM */

    Transport* modem = modem_transport_create(transport, 0);
    CHECK("modem", modem != NULL);

    transport = base64_codec_create(modem);
    CHECK("base64", transport != NULL);

printf("A\n");
    transport = thingstream_transport_create(transport, thingstream_buffer, THINGSTREAM_BUFFER_LENGTH);
    CHECK("thingstream", transport != NULL);


#if (defined(DEBUG_LOG_CLIENT) && (DEBUG_LOG_CLIENT > 0))
printf("B\n");
    transport = log_client_transport_create(transport, printf, 0xFF);
    CHECK("log_client", transport != NULL);
#endif /* DEBUG_LOG_CLIENT */
printf("C\n");
    printf("D\n");
    Client* client = Client_create(transport, NULL);
    CHECK("client", client != NULL);

printf("after");
    if (client != NULL)
    {
        Topic topic;

        cr = Client_connect(client, true, NULL, NULL);
        CHECK("connect", cr == CLIENT_SUCCESS);

        /* Registration is redundant here, since subscribeName can also return the Id.
         * Typical applications might not subscribe to topics they publish to, so this
         * is left here for illustrative purposes. */
        cr = Client_register(client, EXAMPLE_TOPIC, &topic);
        CHECK("register", cr == CLIENT_SUCCESS);
        exampleTopicId = topic.topicId;

        Client_set_subscribe_callback(client, receiveCallback, NULL);

        /* subscribe to the same message to receive it back by the server */
        cr = Client_subscribeName(client, EXAMPLE_TOPIC, MQTT_QOS1, NULL);
        CHECK("subscribe", cr == CLIENT_SUCCESS);

        char *msg = "Hello from Paul's Raspberry Pi";
        cr = Client_publish(client, topic, MQTT_QOS1, false, (uint8_t*) msg, strlen(msg), NULL);
        CHECK("publish", cr == CLIENT_SUCCESS);
    }

    while (!done)
    {
        /* poll for incoming messages */
        Client_run(client, 0);
    }

    printf("Response received; disconnecting ...\n");
    cr = Client_disconnect(client, 0);
    CHECK("disconnect", cr == CLIENT_SUCCESS);
    return 0;

error:
    return 1;
}

