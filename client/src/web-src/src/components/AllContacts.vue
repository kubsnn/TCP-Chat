<script setup>
import Contact from '@/components/Contact.vue'
import AddContact from '@/components/AddContact.vue'
import { FwbButton, FwbHeading, FwbInput, FwbSidebar, FwbSidebarItem, FwbSidebarDropdownItem } from 'flowbite-vue'
import { eventBus } from '@/services/EventBus';
import { ref } from 'vue'

const isShowModal = ref(false)

function closeModal() {
    isShowModal.value = false
}
function showModal() {
    isShowModal.value = true
}

const emits = defineEmits(['refresh-friends'])


</script>

<script>

export default {
    components: {
        Contact
    },
    props: {
        contacts: {
            type: Array,
            default: []
        }
    },
    mounted() {
        this.refreshInvitations();
    },
    data() {
        return {
            pendingInvites: [],
        };
    },
    methods: {
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
    },
}
</script>


<template>
    <fwb-button @click="showModal" color="pink" class="w-full mb-2"> Add Friend </fwb-button>
    <AddContact :isShowModal="isShowModal" :closeModal="closeModal" :friendsList="contacts"
        @refresh-friends="emits('refresh-friends')" />



    <ul class="w-full h-full flex flex-col m-0 p-2 mb-2 rounded-lg bg-slate-200 dark:bg-slate-800"
        v-if="contacts.length > 0">
        <Contact v-for="contact in contacts" :key="contact.name" :name="contact.name" :active="contact.active"
            class="w-full rounded-lg p-2 mb-2 flex"
            :class="{ 'bg-pink-600 dark:bg-pink-600': contact.active, 'bg-pink-300 dark:bg-pink-950': !contact.active }" />
    </ul>

    <fwb-button @click="refreshInvitations" color="pink" class="w-full mb-2"> Refresh Invites </fwb-button>


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
