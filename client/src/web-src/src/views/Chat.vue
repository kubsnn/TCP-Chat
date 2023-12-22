<template>
    <div class="flex flex-row justify-start w-full h-full">
        <div class="p-2" :class="activeContact == null ? 'w-full' : 'w-64 grow-0'">
            <fwb-tabs v-model="activeTab" class="p-5" color="pink">
                <fwb-tab name="contacts" title="Contacts">
                    <AllContacts :contacts="contacts" :friends="friends" :onlineUsers="onlineUsers"
                        :activeContact="activeContact" @refresh-friends="refreshFriends" @open-chat="setActiveContact"
                        @get-messages="receive_new_message" />
                </fwb-tab>
                <fwb-tab name="settings" title="Settings">
                    <HamburgerMenu />
                </fwb-tab>
            </fwb-tabs>
        </div>
        <div v-if="activeContact !== null" class="grow  overflow-x-hidden p-2">
            <Conversation :contact="activeContact" :online="contacts.find(contact => contact.name === activeContact).online"
                :messages="messages" />
        </div>
    </div>
</template>

<script>
import AllContacts from '../components/AllContacts.vue';
import Conversation from '../components/Conversation.vue';
import HamburgerMenu from '../components/HamburgerMenu.vue';
import { FwbTab, FwbTabs } from 'flowbite-vue';

export default {
    components: {
        FwbTab,
        FwbTabs,
        AllContacts,
        Conversation,
        HamburgerMenu,
    },
    data() {
        return {
            contacts: [],
            onlineUsers: [],
            messages: [],
            activeTab: 'contacts',
            activeContact: null,
            username: this.$store.getters.getLoggedUser
        };
    },
    methods: {
        getFriends() {
            eel.get_friends()(this.getFriendsHandler);
        },

        // Your existing getOnlineUsers method
        getOnlineUsers() {
            eel.get_online_users()(this.getOnlineUsersHandler);
        },

        // Your existing getFriendsHandler method
        getFriendsHandler(response) {
            // [{'username': 'Mateusz', 'online': False}]
            // Add friends to contacts if they are not already there
            response.forEach((friend) => {
                if (!this.contacts.some((contact) => contact.name === friend.username)) {
                    this.contacts.push({ name: friend.username, online: friend.online });
                }
            });

            // Remove contacts that are not friends anymore
            this.contacts.forEach((contact, index) => {
                if (!response.some((friend) => friend.username === contact.name)) {
                    this.contacts.splice(index, 1);
                }
            });

            console.log("Contacts: ", this.contacts);
        },

        // Your existing getOnlineUsersHandler method
        getOnlineUsersHandler(response) {
            // ['Aszyk']
            // Set online status for contacts
            this.contacts.forEach((contact) => {
                if (response.some((onlineUser) => onlineUser === contact.name)) {
                    contact.online = true;
                } else {
                    contact.online = false;
                }
            });

            console.log("Contacts: ", this.contacts);
        },

        // Your existing refreshFriends method
        refreshFriends() {
            console.log("Refreshing friends");
            this.getFriends();
            this.getOnlineUsers();
        },

        setActiveContact(contact) {
            console.log("Setting active contact to: ", contact);
            this.refreshFriends();
            this.activeContact = contact;
            eel.get_messages(this.username, this.activeContact)(this.update_messages);
        },

        receive_new_message(contact) {
            eel.get_messages(this.username, contact)(this.update_messages);
        },

        update_messages(messages) {

            //reverse array to show newest messages at the bottom
            const mappedMessages = messages.map((message) => {
                return {
                    id: message[0],
                    message: message[2],
                    isResponder: message[3] === 0,
                    timestamp: message[4],
                }
            });

            mappedMessages.reverse();
            console.log("Mapped messages: ", mappedMessages);
            this.refreshFriends();

            this.messages = mappedMessages
        }
    },
    beforeMount() {
        eel.expose(
            this.update_messages,
            'update_messages'
        );
        this.getFriends();
        this.getOnlineUsers();
    },
    mounted() {
        console.log("Logged in as: ", this.username);
        this.refreshFriends();

        //refresh online status every 60 sec
        setInterval(() => {
            this.refreshFriends();
        }, 60000);
    },
};
</script>
