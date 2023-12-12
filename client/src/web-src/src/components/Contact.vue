<template>
    <li :class="getListItemClasses">
        <fwb-avatar :img="avatarURL" />
        <fwb-p weight="bold" class="grow px-2 text-center">{{ name }}</fwb-p>
    </li>
</template>

<script>
import { FwbAvatar, FwbP } from 'flowbite-vue';

export default {
    components: {
        FwbAvatar,
        FwbP
    },
    props: {
        active: {
            type: Boolean,
            default: false
        },
        avatarURL: {
            type: String,
            default: '/images/avatar-1.jpg'
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
                'justify-between': true,
                'items-center': true,
                'bg-pink-600 dark:bg-pink-600': this.active,
                'bg-pink-300 dark:bg-pink-950': !this.active
            };
        }
    }
};
</script>
