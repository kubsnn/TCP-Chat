<script setup>
import { FwbButton, FwbHeading } from 'flowbite-vue'

</script>

<script>
export default {
    methods: {
        changeRoute(newRoute) {
            this.$router.push(newRoute);

        },
    },
};
</script>

<template>
    <div class="flex flex-col mx-auto w-full h-full justify-center align-middle gap-4">
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
        </nav>
    </div>
</template>

