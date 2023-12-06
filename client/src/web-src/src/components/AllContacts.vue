<script setup>
import { ref } from 'vue'
import { FwbP } from 'flowbite-vue'
import { FwbAvatar } from 'flowbite-vue'

//const contacts = eel.getContacts()

const contacts = ref([{ name: 'contact1', active: false }, { name: 'contact2', active: true }, { name: 'contact2', active: false }, { name: 'contact2', active: false }]);
</script>


<template>
    <ul class="w-full h-full flex flex-col m-0 p-2 rounded-lg bg-slate-200 dark:bg-slate-800">
        <li class="w-full bg-pink-600 dark:bg-pink-600 rounded-lg p-2 mb-2 flex">
            <fwb-avatar img="/images/avatar-1.jpg" /> <fwb-p weight="bold" align="center">contact0</fwb-p>
        </li>
        <li v-for="contact in    contacts   " class="w-full rounded-lg p-2 mb-2 flex ">
            {{ contact.active ? "bg-pink-600 dark:bg-pink-600" : "bg-pink-300 dark:bg-pink-950" }}
            <fwb-avatar img="/images/avatar-1.jpg" /> <fwb-p weight="bold" align="center">{{ contact.name }}</fwb-p>
        </li>
    </ul>
</template>