<template>
    <div class="flex w-full justify-start  p-2" :class="mContainerClass">
        <div :class="messageClass">
            <p class="text-white w-full text-pretty text-ellipsis break-words overflow-hidden">{{ message }}</p>
        </div>
    </div>
</template>

<script>
export default {
    props: {
        message: {
            type: String,
            required: true
        },
        isResponder: {
            type: Boolean,
            default: false
        }
    },
    computed: {
        messageClass() {
            return [
                'p-2 rounded-lg ',
                this.isResponder ? 'bg-pink-300 text-end dark:bg-pink-950' : 'bg-pink-600 text-start  dark:bg-pink-600'
            ];
        },
        mContainerClass() {
            return [
                'flex',
                this.isResponder ? 'flex-row pr-10' : 'flex-row-reverse pl-10'
            ];
        }
    }
};
</script>

<style scoped>
/* Add Tailwind CSS classes here */
</style>
