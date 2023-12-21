
<template>
    <fwb-button @click="showModal" color="pink" class="w-full mb-2"> Manage Friends </fwb-button>
    <AddContact :isShowModal="isShowModal" :closeModal="closeModal" :friendsList="contacts"
        @refresh-friends="emits('refresh-friends')" />

    <div class="w-full h-full flex flex-col m-0 p-2 mb-2 gap-3 rounded-lg bg-slate-200 dark:bg-slate-800"
        v-if="contacts.length > 0">
        <fwb-button v-for="contact in contacts" :key="contact.name" class="w-full text-left p-3 " color="dark"
            @click="openChat(contact)" :outline="contact.name === activeContact">
            <div class="w-full flex flex-nowrap justify-between items-center">

                <span class="grow text-left pr-1">{{ contact.name }}</span>
                <font-awesome-icon icon="fa-solid fa-message" v-if="notifications.includes(contact.name)"
                    class="text-blue-400 text-sm pr-2 fa-shake" />
                <font-awesome-icon v-if="contact.online" icon="fa-solid fa-plug" class="text-green-400 text-sm" />
                <font-awesome-icon v-else icon="fa-solid fa-plug" class="text-red-400 text-sm" />
            </div>
        </fwb-button>
    </div>

    <fwb-sidebar-dropdown-item @click="refreshInvitations">
        <template #trigger> Invites
            <span v-if="pendingInvites.length > 0" class="text-xs px-2 text-gray-400">({{ pendingInvites.length }})</span>
        </template>
        <template #icon>
            <font-awesome-icon class="w-5 h-4" icon="fa-solid fa-envelope" />
        </template>
        <template #default>
            <fwb-sidebar-item v-for="invite in pendingInvites" :key="invite.username" :name="invite.username" link=""
                class="w-full rounded-lg p-2 mb-2 flex ">
                <template #default>
                    <div class="flex justify-start items-center">
                        <span class="grow">{{ invite.username }}</span>
                        <fwb-button @click="acceptInvite(invite.username)" color="green" pill square class="mr-1">
                            <font-awesome-icon icon="fa-solid fa-check" class="w-5 h-4" />

                        </fwb-button>
                        <fwb-button @click="rejectInvite(invite.username)" color="red" pill square class="mr-1">
                            <font-awesome-icon icon="fa-solid fa-xmark" class="w-5 h-4" />

                        </fwb-button>
                    </div>

                </template>
            </fwb-sidebar-item>
        </template>
    </fwb-sidebar-dropdown-item>
</template>



<script>
import Contact from '@/components/Contact.vue'
import AddContact from '@/components/AddContact.vue'
import { FwbButton, FwbSidebarItem, FwbSidebarDropdownItem } from 'flowbite-vue'

export default {
    components: {
        Contact,
        AddContact,
        FwbButton,
        FwbSidebarItem,
        FwbSidebarDropdownItem
    },
    props: {
        contacts: {
            type: Array,
            default: []
        },
        activeContact: {
            type: String,
            default: ''
        },
    },
    emits: {
        'refresh-friends': null,
        'open-chat': null,
        'get-messages': null
    },
    mounted() {
        eel.expose(
            this.message_from,
            'message_from'
        );
        this.refreshInvitations();
        setInterval(this.refreshInvitations, 30000);
    },
    data() {
        return {
            pendingInvites: [],
            isShowModal: false,
            notifications: []
        };
    },
    methods: {
        message_from(contact_name) {
            if (contact_name === this.activeContact) {
                this.$emit('get-messages', contact_name)
            }
            else {
                //new message notification
                if (this.notifications.includes(contact_name)) {
                    this.notifications.splice(this.notifications.indexOf(contact_name), 1);
                }
                this.notifications.push(contact_name);
                //play notification sound
                var audio = new Audio('/notification.mp3');
                audio.play();
                audio.volume = 0.5;
            }
        },
        closeModal() {
            this.isShowModal = false
            this.refreshInvitations();

        },
        showModal() {
            this.isShowModal = true
            this.refreshInvitations();

        },
        handlePendingInvites(response) {
            console.log(response);
            this.pendingInvites = response;
        },
        async refreshInvitations() {
            await eel.get_pending_invites()(this.handlePendingInvites);
            this.$emit('refresh-friends');

        },
        async acceptInvite(username) {
            await eel.accept_invite(username)(this.refreshInvitations);
        },
        async rejectInvite(username) {
            await eel.reject_invite(username)(this.refreshInvitations);
        },
        openChat(contact) {
            if (this.notifications.includes(contact.name)) {
                this.notifications.splice(this.notifications.indexOf(contact.name), 1);
            }
            this.$emit('open-chat', contact.name);
        }
    },
}
</script>

