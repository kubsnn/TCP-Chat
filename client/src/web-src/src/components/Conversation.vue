
<template>
    <div class="w-full h-full flex flex-col  bg-slate-200 dark:bg-slate-800 rounded-lg">

        <div class="w-full h-16 flex-none  flex flex-row justify-between items-center p-2 text-black dark:text-white">
            <fwb-heading class="pl-4" tag="h3" align="Left">{{ username }}</fwb-heading>

            <fwb-badge v-if="online" size="xs" type="green">Online</fwb-badge>
            <fwb-badge v-else size="xs" type="red">Offline</fwb-badge>
        </div>
        <div class="w-full grow overflow-x-hidden overflow-y-auto flex flex-col-reverse items-start p-2">
            <message-bubble v-if="messages.length > 0" v-for="message in messages" :key="message.id"
                :message="message.message" :isResponder="message.isResponder" />
            <message-bubble v-else :message="'Start Your Conversation'" :isResponder="false" />
        </div>
        <div class="w-full h-16 flex-none flex flex-row flex-nowrap justify-between items-center p-2">
            <fwb-input class="w-full" placeholder="Type a message..." v-model="currentMessage" ref="input"
                @keyup.enter.exact.stop.prevent="sendMessage" />
            <fwb-button @click="sendMessage" color="pink" pill class="ml-2">
                <span class=" flex flex-nowrap ">
                    Send
                    <font-awesome-icon class="w-5 h-4" icon="fa-solid fa-paper-plane" />
                </span>
            </fwb-button>
        </div>
    </div>
</template>


<script>
import { FwbButton, FwbHeading, FwbInput, FwbBadge } from 'flowbite-vue'
import MessageBubble from '@/components/MessageBubble.vue'

export default {
    components: {
        FwbButton,
        FwbHeading,
        FwbInput,
        FwbBadge,
        MessageBubble
    },
    data() {
        return {
            currentMessage: '',
        };
    },
    props: {
        contact: {
            type: String,
            default: 'contact1'
        },
        online: {
            type: Boolean,
            default: false
        },
        messages: {
            type: Array,
            default: []
        }
    },
    methods: {
        async sendMessage() {
            console.log('send message', this.currentMessage)
            await eel.send_message(this.$store.getters.getLoggedUser, this.contact, this.currentMessage)(this.handleResponse);
            //focus on input
            this.$refs.input.focus()

        },
        handleResponse(response) {
            console.log(response)
            this.currentMessage = ''
        },
        packMessage(message) {
            return {
                timestamp: new Date(),
                message: message,
            }
        }
    }
}
</script>

