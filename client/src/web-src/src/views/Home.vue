<script setup>
import { FwbButton, FwbHeading } from 'flowbite-vue'
</script>

<script>
export default {
    name: 'Home',
    data() {
        return {
            isConnected: null,
        }
    },
    methods: {
        changeRoute(newRoute) {
            this.$router.push(newRoute);
        },
        async connect() {
            let connectionSocketStatus = await eel.connect_to_server()()
            if (connectionSocketStatus === true) {
                this.setConnected()
            }
            else {
                this.setDisconnected()
            }
        },
        setConnected() {
            this.$store.dispatch('connected')
            this.isConnected = true
        },
        setDisconnected() {
            this.$store.dispatch('disconnected')
            this.isConnected = false
        },
        getConnectionStatus() {
            return this.$store.state.isConnected
        }
    },
    mounted() {
        this.isConnected = this.getConnectionStatus()
        if (this.isConnected != true) {
            this.connect()
        }
    }
};
</script>

<template>
    <div v-if="isConnected === true" class="flex flex-col mx-auto w-full h-full justify-center align-middle gap-4">
        <fwb-heading class="text-center" tag="h2">Welcome!</fwb-heading>
        <fwb-heading class="text-center p-4" tag="h1">😄</fwb-heading>
        <nav class="text-lg flex justify-center gap-2">
            <fwb-button color="pink" @click="changeRoute('/login')">Login
                <template #suffix><font-awesome-icon icon="fa-solid fa-right-to-bracket" />
                </template>
            </fwb-button>
            <fwb-button color="pink" @click="changeRoute('/register')">Register
                <template #suffix>
                    <font-awesome-icon icon="fa-solid fa-plus" />
                </template>
            </fwb-button>
            <fwb-button color="pink" @click="changeRoute('/chat')">Chat

            </fwb-button>
        </nav>

    </div>
    <div v-else-if="isConnected === false" class="flex flex-col mx-auto w-full h-full justify-center align-middle gap-4">
        <fwb-heading class="text-center" tag="h2">Could Not Connect to Server</fwb-heading>
        <fwb-heading class="text-center p-4" tag="h1">🤔</fwb-heading>
        <fwb-heading class="text-center" tag="h2">Please check your connection and try again</fwb-heading>

    </div>
    <div v-else class="flex flex-col mx-auto w-full h-full justify-center align-middle gap-4">
        <fwb-heading class="text-center" tag="h2">Connecting To Server</fwb-heading>
        <fwb-heading class="text-center text-white" tag="h2"><font-awesome-icon icon="fa-solid fa-spinner"
                spin-pulse /></fwb-heading>
    </div>
</template>

