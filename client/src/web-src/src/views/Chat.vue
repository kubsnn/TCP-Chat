<script setup>
import AllContacts from '../components/AllContacts.vue';
import Conversation from '../components/Conversation.vue';
</script>

<template>
    <div class="flex flex-row justify-start w-full h-full">

        <div class="w-64 p-2">
            <AllContacts />

        </div>
        <div class=" grow p-2">
            <Conversation />

        </div>
    </div>
</template>
