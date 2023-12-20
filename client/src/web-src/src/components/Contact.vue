<template>
    <fwb-button class="w-full" gradient="pink" outline shadow="{{this.active ? 'pink' : null}}">
        {{ name }}</fwb-button>
</template>

<script>
import { FwbButton } from 'flowbite-vue';

export default {
    components: {
        FwbButton
    },
    props: {
        active: {
            type: Boolean,
            default: false
        },
        name: {
            type: String,
            default: 'contact1'
        }
    },
    computed: {
        getListItemClasses() {
            return {
                'w-full': true,
                'rounded-lg': true,
                'p-2': true,
                'mb-2': true,
                'flex': true,
                'justify-around': true,
                'items-center': true,
                'bg-pink-600 dark:bg-pink-600': this.active,
                'bg-pink-300 dark:bg-pink-950': !this.active
            };
        }
    }
};
</script>
