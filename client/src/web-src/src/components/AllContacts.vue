
<template>
    <fwb-button @click="showModal" color="pink" class="w-full mb-2"> Manage Friends </fwb-button>
    <AddContact :isShowModal="isShowModal" :closeModal="closeModal" :friendsList="contacts"
        @refresh-friends="emits('refresh-friends')" />

    <div class="w-full h-full flex flex-col m-0 p-2 mb-2 gap-3 rounded-lg bg-slate-200 dark:bg-slate-800"
        v-if="contacts.length > 0">
        <fwb-button v-for="contact in contacts" :key="contact.name" class="w-full text-left p-3 " color="dark"
            @click="openChat(contact)" :outline="contact.name === activeContact">
            <div class="w-full flex flex-nowrap justify-between items-center">

                <span class="grow text-left">{{ contact.name }}</span>
                <font-awesome-icon v-if="contact.online" icon="fa-solid fa-plug" class="text-green-400 text-sm" />
                <font-awesome-icon v-else icon="fa-solid fa-plug" class="text-red-400 text-sm" />
            </div>
        </fwb-button>
    </div>

    <fwb-sidebar-dropdown-item>
        <template #trigger> Invites </template>
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
        'open-chat': null
    },
    mounted() {
        this.refreshInvitations();
    },
    data() {
        return {
            pendingInvites: [],
            isShowModal: false
        };
    },
    methods: {
        closeModal() {
            this.isShowModal = false
        },
        showModal() {
            this.isShowModal = true
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
            console.log(contact);
            this.$emit('open-chat', contact.name);
        }
    },
}
</script>

