
<template>
    <div class="w-full h-full flex flex-col m-0 p-2 rounded-lg bg-slate-200 dark:bg-slate-800">

        <fwb-sidebar-item @click="logout" link="">
            <template #icon>
                <font-awesome-icon class="w-5 h-4" icon="fa-solid fa-right-from-bracket" />
            </template>
            <template #default>Log Out</template>
        </fwb-sidebar-item>



    </div>
</template>

<script>
import { FwbButton, FwbHeading, FwbInput, FwbSidebar, FwbSidebarItem, FwbSidebarDropdownItem } from 'flowbite-vue'
import { eventBus } from '@/services/EventBus';



export default {
    components: {
        FwbButton,
        FwbHeading,
        FwbInput,
        FwbSidebar,
        FwbSidebarItem,
        FwbSidebarDropdownItem
    },
    data() {
        return {
            username: '',
        };
    },
    methods: {
        logout() {
            //prevent default
            console.log('logout')
            eel.logout_from_server()(this.handleResponse);
        },
        handleResponse(response) {
            try {
                if (response == true) {
                    eventBus.emit('show-toast', 'success', 'Successfully logged out', 5000);
                    this.$router.push('/login')
                }
                else {
                    eventBus.emit('show-toast', 'danger', 'Error logging out', 5000);
                }
            }
            catch (error) {
                console.log(error)
                eventBus.emit('show-toast', 'danger', 'Error logging out', 5000);
                return
            }


        },

    },

};
</script>
