<script setup>
import { FwbButton, FwbHeading, FwbInput, FwbBadge } from 'flowbite-vue'
import { eventBus } from '@/services/EventBus';
import MessageBubble from '@/components/MessageBubble.vue'

</script>

<script>
export default {
    data() {
        return {
            username: '',
            password: '',
        };
    },
    props: {
        username: {
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
        sendMessage() {
            console.log('send message')
        }
    }
}
</script>


<template>
    <div class="w-full h-full flex flex-col  bg-slate-200 dark:bg-slate-800 rounded-lg">

        <div class="w-full h-16 flex-none  flex flex-row justify-between items-center p-2 text-black dark:text-white">
            <fwb-heading tag="h3" align="Left">{{ username }}</fwb-heading>

            <fwb-badge v-if="online" size="xs" type="green">Online</fwb-badge>
            <fwb-badge v-else size="xs" type="red">Offline</fwb-badge>
        </div>
        <div class="w-full grow flex flex-col-reverse items-start p-2">
            <message-bubble v-for="message in messages" :key="message.id" :message="message.message"
                :isResponder="message.isResponder" />
        </div>
        <div class="w-full h-16 flex-none flex flex-row flex-nowrap justify-between items-center p-2">
            <fwb-input class="w-full" placeholder="Type a message..." @keyup.enter="sendMessage()" />
            <fwb-button @click="sendMessage()" color="pink" pill class="ml-2">
                <span class=" flex flex-nowrap ">
                    Send
                    <font-awesome-icon class="w-5 h-4" icon="fa-solid fa-paper-plane" />
                </span>
            </fwb-button>
        </div>
    </div>
</template>