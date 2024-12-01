const binding = require('../build/zoin.node');

export interface Zoin {
    create_window(title: string, url: string): string;
}

export var Zoin: {
    new(): Zoin; 
} = binding.Zoin;