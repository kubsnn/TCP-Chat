<script>
import { FwbButton, FwbModal, FwbInput, FwbListGroup, FwbListGroupItem, FwbBadge } from 'flowbite-vue'
import { eventBus } from '@/services/EventBus';


export default {
    components: {
        FwbButton,
        FwbModal,
        FwbInput,
        FwbListGroup,
        FwbListGroupItem,
        FwbBadge
    },
    data() {
        return {
            queryResults: [],
        };
    },
    props: {
        isShowModal: {
            type: Boolean,
            default: false
        },
        closeModal: {
            type: Function,
            default: () => { }
        },
        friendsList: {
            type: Array,
            default: []
        }
    },
    methods: {
        onQueryUpdate(e) {
            console.log(e.target.value);
            this.getSearchUsers(e.target.value);
        },
        async getSearchUsers(value) {
            await eel.get_search_users(value)(this.handleSearch);
        },
        handleSearch(response) {
            this.queryResults = response;
        },
        async handleUserClick(user) {
            console.log(user);
            console.log(this.friendsList);

            if (!this.friendsList.some((friend) => friend.name === user)) {
                await eel.add_friend(user)(this.$emit('refresh-friends'));
            } else {
                await eel.remove_friend(user)(this.$emit('refresh-friends'));
            }
        }
    },
};
</script>


<template>
    <fwb-modal v-if="isShowModal" @close="closeModal">
        <template #header>
            <div class="flex items-center text-lg text-pink-900 dark:text-pink-300">
                Add Friends
            </div>
        </template>
        <template #body>

            <fwb-input label="Search for username" placeholder="Enter username" size="lg" @input="onQueryUpdate">
                <template #prefix>
                    <font-awesome-icon class="w-5 h-4" icon="fa-solid fa-search" />
                </template>
            </fwb-input>

            <fwb-list-group v-if="queryResults.length > 0" class="mt-4 mb-4 w-full">
                <fwb-list-group-item hover v-for="result in queryResults" :key="result.username"
                    @click="handleUserClick(result.username)" class="group w-full p-2 flex flex-row">
                    <span class="grow">{{ result.username }}</span>
                    <div class="mx-4">
                        <fwb-badge type="green" v-if="result.online">Online</fwb-badge>
                        <fwb-badge type="red" v-else>Offline</fwb-badge>
                    </div>
                    <template #suffix>
                        <font-awesome-icon class="w-5 h-4 group-hover:text-green-500" icon="fa-solid fa-user-plus"
                            v-if="!friendsList.some(friend => friend.name === result.username)" />
                        <font-awesome-icon class="w-5 h-4 group-hover:text-red-500" icon="fa-solid fa-user-minus" v-else />
                    </template>
                </fwb-list-group-item>
            </fwb-list-group>

        </template>
    </fwb-modal>
</template>