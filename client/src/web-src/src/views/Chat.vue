<template>
    <div class="flex flex-row justify-start w-full h-full">
        <div class="w-64 p-2">
            <fwb-tabs v-model="activeTab" class="p-5" color="pink">
                <fwb-tab name="contacts" title="Contacts">
                    <AllContacts :contacts="contacts" :friends="friends" :onlineUsers="onlineUsers"
                        @refresh-friends="refreshFriends" />
                </fwb-tab>
                <fwb-tab name="settings" title="Settings">
                    <HamburgerMenu />
                </fwb-tab>
            </fwb-tabs>
        </div>
        <div class="grow p-2">
            <Conversation v-if="activeContact !== null" :username="contacts[activeContact].name"
                :online="contacts[activeContact].online" :messages="messages" />
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
            friends: [],
            onlineUsers: [],
            messages: [
                { id: 1, message: 'years old. Richard McClintock, a Latin professor at Hampden-Sydney College in Virginia, looked up one of the more obscure Latin words, consectetur, from a Lorem Ipsum passage, and going through the cites of the word in classical literature, discovered the undoubtable source. Lorem Ipsum comes from sections 1.10.32 and 1.10.33 of "de Finibus Bonorum et Malorum" (The Extremes of Good and Evil) by Cicero, written in 45 BC. This book is a treatise on the theory of eth', isResponder: false },
                { id: 2, message: 'Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.', isResponder: true },
                { id: 3, message: 'There are many variations of passages of Lorem Ipsum available,', isResponder: false },
                { id: 4, message: 'It uses a dictionary of over 200 Latin words, combined with a handful of model sentence structures, to generate Lorem Ipsum which looks reasonable.', isResponder: true }
            ],
            activeTab: 'contacts',
            activeContact: null,
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
                    this.contacts.push({ name: friend.username, active: false, online: friend.online });
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
            //this.getOnlineUsers();
        },
    },
    beforeMount() {
        this.getFriends();
        this.getOnlineUsers();
    },
};
</script>
